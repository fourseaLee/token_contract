#ifndef HTTP_SEVER_H
#define HTTP_SEVER_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/queue.h>
#include <string>
#include <vector>
#include <event.h>
#include <evhttp.h>
#include <event2/keyvalq_struct.h>
#include <event2/event.h>
#include <event2/http.h>
#include <event2/buffer.h>
#include <event2/util.h>
#include <event2/keyvalq_struct.h>
#include <functional>
#include <memory>

struct evhttp_request;
class HTTPRequest;
using HTTPRequestHandler = std::function<void(std::unique_ptr<HTTPRequest> req)>;


struct HTTPPathHandler
{
    HTTPPathHandler() {}
    HTTPPathHandler(std::string _prefix, HTTPRequestHandler _handler):prefix(_prefix), handler(_handler){}
    std::string prefix;
    HTTPRequestHandler handler;
};

extern std::vector<HTTPPathHandler> g_path_handlers;

class HTTPRequest
{
private:
    struct evhttp_request* req;
public:
    HTTPRequest(struct evhttp_request* req);
    ~HTTPRequest();

    enum RequestMethod
    {
        UNKNOWN,
        GET,
        POST,
        HEAD,
        PUT,
        OPTIONS
    };

    std::string GetURI();

    RequestMethod GetRequestMethod();

    std::string GetHeader();

    std::string ReadBody();

    void WriteHeader(const std::string& hdr, const std::string& value);

    void WriteReply(int nStatus, const std::string& strReply = "");
};

void RegisterHTTPHandler(const std::string &prefix, const HTTPRequestHandler &handler);

void SignalHandler(int sig);

void HttpRequestCb(struct evhttp_request *req, void *arg);

/**/
void SendTransaction(std::unique_ptr<HTTPRequest> req);

void GetUtxo(std::unique_ptr<HTTPRequest> req);

void QueryBalance(std::unique_ptr<HTTPRequest> req);



#endif // HTTP_SEVER_H
