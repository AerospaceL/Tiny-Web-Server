#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <thread>
#include <condition_variable>
#include <mutex>
#include <vector>
#include <queue>
#include <future>

class ThreadPool{
private:
    std::vector<std::thread> m_pool;
    std::queue<std::function<void()>> tasks;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    bool m_stop;

public:
    ThreadPool(size_t threadNumber):m_stop(true){
        for(size_t i = 0; i < threadNumber; i++){
            m_pool.emplace_back(
                [this](){
                    while(true){
                        std::function<void()> task;
                        {
                            std::unique_lock<std::mutex> lk(m_mutex);
                            // 当服务器运行状态非停且任务队列非空时等待锁
                            m_cv.wait(lk, [this](){ return m_stop || !tasks.empty(); });
                            if(m_stop && tasks.empty())  return;
                            task = std::move(tasks.front());
                            tasks.pop();
                        }
                        task();
                    }
                }
            );
        }
    }

    ~ThreadPool(){
        {
            std::unique_lock<std::mutex> lk(m_mutex);
            m_stop = true;
        }
        m_cv.notify_all();
        for(auto& thread : m_pool){
            thread.join();
        }
    }

    template<typename F, typename...Args>
    auto submit(F&& f, Args&&... args) -> std::future<decltype(f(args...))>{
        // 创建一个函数包含其参数，准备执行
        std::function<decltype(f(args...))()> func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        // 封装这个函数成智能指针便于它的拷贝
        auto task_ptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(func);
        // 将任务封装成空函数便于声明统一类型的容器
        {
            std::unique_lock<std::mutex> lk(m_mutex);
            if(!m_stop){
                std::function<void()> wrapper_func = [task_ptr]{
                    (*task_ptr)();
                };
                tasks.emplace(wrapper_func);
            }
            else throw std::runtime_error("Submit task on stopped threadpool!");
        }
        // 唤醒某个阻塞的进程
        m_cv.notify_all();
        return task_ptr->get_future();

    }
};

#endif