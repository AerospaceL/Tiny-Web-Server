#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <regex>

#include "buffer.h"

class HTTPrequest
{
private:
    bool parseRequestLine(const std::string& line); /* 解析请求行 */
    void parseRequestHeader(const std::string& line); /* 解析请求头 */
    void parseDataBody(const std::string& line); /* 解析数据体 */


    void parsePath();
    void parsePost();

    static int convertHex(char ch);

    PARSE_STATE state;
    std::string method, path, version, body;
    std::unordered_map<std::string,std::string> header;
    std::unordered_map<std::string,std::string> post;

    static const std::unordered_set<std::string> DEFAULT_HTML;
   
public:
    enum PARSE_STATE{
        REQUEST_LINE,
        HEADERS,
        BODY,
        FINISH,
    };

    enum HTTP_CODE {
        NO_REQUEST = 0,
        GET_REQUEST,
        BAD_REQUEST,
        NO_RESOURSE,
        FORBIDDENT_REQUEST,
        FILE_REQUEST,
        INTERNAL_ERROR,
        CLOSED_CONNECTION,
    };

    HTTPrequest() { init(); };
    ~HTTPrequest() = default;

    void init();
    bool parse(Buffer& buff); /* 解析HTTP请求 */

    /* 获取HTTP信息 */
    std::string path() const;
    std::string& path();
    std::string method() const;
    std::string version() const;
    std::string getPost(const std::string& key) const;
    std::string getPost(const char* key) const;

    bool isKeepAlive() const;

};

#endif