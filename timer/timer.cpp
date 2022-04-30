#include "timer.h"

void TimerManager::siftup(size_t i)
{
    assert(i >= 0 && i < timerHeap.size());
    /* j是i的父节点 */
    size_t j = (i - 1) / 2;
    while(j >= 0) {
        if(timerHeap[j] < timerHeap[i]) { break; }
        swapNode(i, j);  /* 子节点上筛 */
        i = j;
        j = (i - 1) / 2;
    }
}

bool TimerManager::siftdown(size_t index, size_t n)
{
    assert(index >= 0 && index < timerHeap.size());
    assert(n >= 0 && n <= timerHeap.size());
    size_t i = index;
    /* j是i的左子节点 */
    size_t j = i * 2 + 1;
    while(j < n) {
        /* 令j指向左右子节点中的较小者 */
        if(j + 1 < n && timerHeap[j + 1] < timerHeap[j]) j++;
        if(timerHeap[i] < timerHeap[j]) break;
        swapNode(i, j); /* 父节点下筛 */
        i = j;
        j = i * 2 + 1;
    }
    /* 如果移动了返回真，否则返回假 */
    return i > index;
}

void TimerManager::swapNode(size_t i, size_t j)
{
    assert(i >= 0 && i < timerHeap.size());
    assert(j >= 0 && j < timerHeap.size());
    std::swap(timerHeap[i], timerHeap[j]);
    ref[timerHeap[i].id] = i;
    ref[timerHeap[j].id] = j;
} 

void TimerManager::addTimer(int id, int timeout, const TimeoutCallBack& cb)
{
    assert(id >= 0);
    size_t i;
    if(ref.count(id) == 0) {
        /* 新节点：堆尾插入，调整堆 */
        i = timerHeap.size();
        ref[id] = i;
        timerHeap.push_back({id, Clock::now() + MS(timeout), cb});
        siftup(i);
    } 
    else {
        /* 已有结点：调整堆 */
        i = ref[id];
        timerHeap[i].expire = Clock::now() + MS(timeout);
        timerHeap[i].cb = cb;
        if(!siftdown(i, timerHeap.size())) {
            siftup(i);
        }
    }
}

void TimerManager::work(int id)
{
    /* 删除指定id结点，并触发回调函数 */
    if(timerHeap.empty() || ref.count(id) == 0) {
        return;
    }
    size_t i = ref[id];
    TimerNode node = timerHeap[i];
    node.cb();
    del(i);
}

void TimerManager::del(size_t index) 
{
    /* 删除指定位置的结点 */
    assert(!timerHeap.empty() && index >= 0 && index < timerHeap.size());
    /* 将要删除的结点换到队尾，然后调整堆 */
    size_t i = index;
    size_t n = timerHeap.size() - 1;
    assert(i <= n);
    if(i < n) {
        swapNode(i, n);
        if(!siftdown(i, n)) {
            siftup(i);
        }
    }
    /* 队尾元素删除 */
    ref.erase(timerHeap.back().id);
    timerHeap.pop_back();
}

void TimerManager::update(int id, int timeout) 
{
    /* 调整指定id的结点 */
    assert(!timerHeap.empty() && ref.count(id) > 0);
    timerHeap[ref[id]].expire = Clock::now() + MS(timeout);;
    siftdown(ref[id], timerHeap.size());
}

void TimerManager::handle_expired_event() 
{
    /* 清除超时结点 */
    if(timerHeap.empty()) {
        return;
    }
    while(!timerHeap.empty()) {
        TimerNode node = timerHeap.front();
        if(std::chrono::duration_cast<MS>(node.expire - Clock::now()).count() > 0) { 
            break; 
        }
        node.cb();
        pop();
    }
}

void TimerManager::pop() 
{
    assert(!timerHeap.empty());
    del(0);
}

void TimerManager::clear() 
{
    ref.clear();
    timerHeap.clear();
}

int TimerManager::getNextHandle() 
{
    handle_expired_event();
    size_t res = -1;
    if(!timerHeap.empty()) {
        res = std::chrono::duration_cast<MS>(timerHeap.front().expire - Clock::now()).count();
        if(res < 0) { res = 0; }
    }
    return res;
}