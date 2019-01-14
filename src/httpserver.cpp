#include "httpserver.h"
#include "json.hpp"
#include "logic.h"
#include<glog/logging.h>
#include "contract/contract.h"
#include <exception>
#include <iostream>
using json = nlohmann::json;

std::vector<HTTPPathHandler> g_path_handlers;
HTTPRequest::HTTPRequest(struct evhttp_request* _req) :req(_req){}
HTTPRequest::~HTTPRequest()
{
    // LOG(INFO) << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\n\n"  ;
}


HTTPRequest::RequestMethod HTTPRequest::GetRequestMethod()
{
    switch (evhttp_request_get_command(req)) {
    case EVHTTP_REQ_GET:
        return GET;
        break;
    case EVHTTP_REQ_POST:
        return POST;
        break;
    case EVHTTP_REQ_HEAD:
        return HEAD;
        break;
    case EVHTTP_REQ_PUT:
        return PUT;
        break;
    case EVHTTP_REQ_OPTIONS:
        return OPTIONS;
        break;
    default:
        return UNKNOWN;
        break;
    }
}

static std::string RequestMethodString(HTTPRequest::RequestMethod m)
{
    switch (m) {
    case HTTPRequest::GET:
        return "GET";
        break;
    case HTTPRequest::POST:
        return "POST";
        break;
    case HTTPRequest::HEAD:
        return "HEAD";
        break;
    case HTTPRequest::PUT:
        return "PUT";
        break;
    case HTTPRequest::OPTIONS:
        return "OPTIONS";
        break;
    default:
        return "unknown";
    }
}


std::string HTTPRequest::GetURI()
{
    return evhttp_request_get_uri(req);
}

std::string HTTPRequest::GetHeader()
{
    std::string urlheader;
    struct evkeyvalq *headers;
    struct evkeyval *header;
    headers = evhttp_request_get_input_headers(req);

    for (header = headers->tqh_first; header;header = header->next.tqe_next)
    {
        urlheader = urlheader + header->key + " : " + header->value + "\n";
    }
    return urlheader;
}

void HTTPRequest::WriteHeader(const std::string& hdr, const std::string& value)
{
    struct evkeyvalq* headers = evhttp_request_get_output_headers(req);
    assert(headers);
    evhttp_add_header(headers, hdr.c_str(), value.c_str());
}

void HTTPRequest::WriteReply(int nStatus, const std::string& strReply)
{
    assert(req);
    struct evbuffer* evb = evhttp_request_get_output_buffer(req);
    assert(evb);
    LOG(INFO) << nStatus << "  :  " << strReply << "\n";
    evbuffer_add(evb, strReply.data(), strReply.size());
    auto req_copy = req;

    evhttp_send_reply(req_copy, nStatus, nullptr, nullptr);
    // Re-enable reading from the socket. This is the second part of the libevent
    // workaround above.
    if (event_get_version_number() >= 0x02010600 && event_get_version_number() < 0x02020001)
    {
        evhttp_connection* conn = evhttp_request_get_connection(req_copy);
        if (conn)
        {
            bufferevent* bev = evhttp_connection_get_bufferevent(conn);
            if (bev)
            {
                bufferevent_enable(bev, EV_READ | EV_WRITE);
            }
        }
    }
}

std::string HTTPRequest::ReadBody()
{
    struct evbuffer* buf = evhttp_request_get_input_buffer(req);
    if (!buf)
    {
        LOG(INFO) << "READ_BODY ERROR 1";
        return "";
    }

    size_t size = evbuffer_get_length(buf);
    const char* data = (const char*)evbuffer_pullup(buf, size);
    if (!data)
    {
        LOG(INFO) << "READ_BODY ERROR 2   " << size;
        return "";
    }
    std::string rv(data, size);
    evbuffer_drain(buf, size);
    LOG(INFO) << "READ_BODY : " << rv;
    return rv;
}


void RegisterHTTPHandler(const std::string &prefix, const HTTPRequestHandler &handler)
{
    g_path_handlers.push_back(HTTPPathHandler(prefix, handler));
}


void SignalHandler(int sig)
{
    switch (sig)
    {
    case SIGTERM:
    case SIGHUP:
    case SIGQUIT:
    case SIGINT:
    {
        event_loopbreak();
    }
        break;
    }
}

void HttpRequestCb(struct evhttp_request *req, void *arg)
{
    if (event_get_version_number() >= 0x02010600 && event_get_version_number() < 0x02020001)
    {
        evhttp_connection* conn = evhttp_request_get_connection(req);
        if (conn)
        {
            bufferevent* bev = evhttp_connection_get_bufferevent(conn);
            if (bev)
            {
                bufferevent_disable(bev, EV_READ);
            }
        }
    }
    std::unique_ptr<HTTPRequest> hreq(new HTTPRequest(req));
    // hreq->WriteReply(HTTP_BADMETHOD);
    LOG(INFO) << "Received a " <<  RequestMethodString(hreq->GetRequestMethod()) << " request for " <<  hreq->GetURI() << " from "; //<<  hreq->GetPeer().ToString();
    if (hreq->GetRequestMethod() == HTTPRequest::UNKNOWN)
    {
        hreq->WriteReply(HTTP_BADMETHOD);
        return;
    }
    if (hreq->GetRequestMethod() == HTTPRequest::OPTIONS)
    {
        hreq->WriteHeader("Access-Control-Allow-Origin", "*");
        hreq->WriteHeader("Access-Control-Allow-Credentials", "true");
        hreq->WriteHeader("Access-Control-Allow-Headers", "access-control-allow-origin,Origin, X-Requested-With, Content-Type, Accept, Authorization");
        hreq->WriteReply(HTTP_OK);
        return ;
    }
    hreq->WriteHeader("Access-Control-Allow-Origin", "*");
    hreq->WriteHeader("Access-Control-Allow-Credentials", "true");
    hreq->WriteHeader("Access-Control-Allow-Headers", "access-control-allow-origin,Origin, X-Requested-With, Content-Type, Accept, Authorization");

    if (hreq->GetRequestMethod() != HTTPRequest::POST)
    {
        hreq->WriteReply(HTTP_BADMETHOD);
        return;
    }

    std::string strURI = hreq->GetURI();
    std::string path;
    std::vector<HTTPPathHandler>::const_iterator i = g_path_handlers.begin();
    std::vector<HTTPPathHandler>::const_iterator iend = g_path_handlers.end();
    for (; i != iend; ++i)
    {
        bool match = (strURI == i->prefix);
        if (match)
        {
            path = strURI;
            break;
        }
    }

    if(i != iend)
    {
        LOG(INFO) << "FOUND_PATH : " << path;
        i->handler(std::move(hreq));
    }
    else
    {
        LOG(INFO) << "NOT_FOUND_PATH : " <<  strURI;
        hreq->WriteReply(HTTP_NOTFOUND);
    }

}

static void replyError(const std::unique_ptr<HTTPRequest>& req,const json& json_error)
{
    LOG(ERROR) << json_error.dump() ;
    req->WriteReply(HTTP_INTERNAL,json_error.dump());
}

void testHttpMethod(std::unique_ptr<HTTPRequest> req)
{
    json json_response;
    json_response["code"] = 0;
    json_response["data"] = 0;

    std::string response = json_response.dump();
    req->WriteHeader("Content-Type", "application/json");
    req->WriteReply(HTTP_OK,response);
}


void issueToken(std::unique_ptr<HTTPRequest> req)
{

    json json_error;
    try
    {
        std::string post_data = req->ReadBody();
        LOG(INFO) << "issueToken revieve:" << post_data;
        json json_data = json::parse(post_data);
        if(!json_data.is_object())
        {
            throw std::runtime_error("error params is not json object");
        }

        Logic logic;
        std::vector<std::string> txs = logic.createContract(json_data);
        json json_txs = json::array();
        for(unsigned int i = 0; i < txs.size(); ++i)
        {
            json_txs.push_back(txs[i]);
        }

        json json_response;
        json_response["code"] = 0;
        json_response["data"] = json_txs;

        std::string response = json_response.dump();
        req->WriteHeader("Content-Type", "application/json");
        req->WriteReply(HTTP_OK,response);
        return ;
    }
    catch(json::exception& e)
    {
        json_error["code"] = 1;
        json_error["data"] = e.what();
    }
    catch(std::exception& e)
    {
        json_error["code"] = 10;
        json_error["data"] = e.what();
    }

    replyError(req,json_error);
}

void transferToken(std::unique_ptr<HTTPRequest> req)
{

    json json_error;
    try
    {
        std::string post_data = req->ReadBody();
        LOG(INFO) << "issueToken revieve:" << post_data;

        json json_data = json::parse(post_data);
        if(!json_data.is_object())
        {
            throw std::runtime_error("error params is not json object");
        }

        Logic logic;
        std::vector<std::string> txs = logic.transferContract(json_data);
        json json_txs = json::array();
        for(unsigned int i = 0; i < txs.size(); ++i)
        {
            json_txs.push_back(txs[i]);
        }

        json json_response;
        json_response["code"] = 0;
        json_response["data"] = json_txs;

        std::string response = json_response.dump();
        req->WriteHeader("Content-Type", "application/json");
        req->WriteReply(HTTP_OK,response);
        return ;
    }
    catch(json::exception& e)
    {
        json_error["code"] = 1;
        json_error["data"] = e.what();
    }
    catch(std::exception& e)
    {
        json_error["code"] = 10;
        json_error["data"] = e.what();
    }

    replyError(req,json_error);

}

void queryToken(std::unique_ptr<HTTPRequest> req)
{
    json json_error;
    try
    {
        std::string post_data = req->ReadBody();
        LOG(INFO) << "issueToken revieve:" << post_data;

        json json_data = json::parse(post_data);
        if(!json_data.is_object())
        {
            throw std::runtime_error("error params is not json object");
        }

        json json_response;
        json_response["code"] = 0;
        Logic logic;
        std::string asset_id = json_data["asset_id"].get<std::string>();
        std::string asset_id_hash ;
        Contract::GetContractHash(asset_id,asset_id_hash);
        json_data["asset_id"] = asset_id_hash;
        uint64_t  amount = logic.queryContract(json_data);
        json_response["data"] = amount;

        std::string response = json_response.dump();
        req->WriteHeader("Content-Type", "application/json");
        req->WriteReply(HTTP_OK,response);
        return ;
    }
    catch(json::exception& e)
    {
        json_error["code"] = 1;
        json_error["data"] = e.what();
    }
    catch(std::exception& e)
    {
        json_error["code"] = 10;
        json_error["data"] = e.what();
    }

    replyError(req,json_error);
}


void ContractOffer(std::unique_ptr<HTTPRequest> req)
{

    json json_error;
    try
    {
        std::string post_data = req->ReadBody();
        LOG(INFO) << "contract offer:" << post_data;

        json json_data = json::parse(post_data);
        if(!json_data.is_object())
        {
            throw std::runtime_error("error params is not json object");
        }

        Logic logic;
        std::string txid;
        bool ret = logic.ContractOffer(json_data,txid);
        if(!ret)
        {
            std::string error_info;
            logic.GetErrorInfo(error_info);
            throw std::runtime_error(error_info);
        }

        json json_response;
        json_response["code"] = 0;
        json_response["data"] = txid;
        std::string response = json_response.dump();
        req->WriteHeader("Content-Type", "application/json");
        req->WriteReply(HTTP_OK,response);
        return ;
    }
    catch(json::exception& e)
    {
        json_error["code"] = 1;
        json_error["data"] = e.what();
    }
    catch(std::exception& e)
    {
        json_error["code"] = 10;
        json_error["data"] = e.what();
    }

    replyError(req,json_error);

}

void AssetDefinition(std::unique_ptr<HTTPRequest> req)
{

    json json_error;
    try
    {
        std::string post_data = req->ReadBody();
        LOG(INFO) << "asset definition:" << post_data;

        json json_data = json::parse(post_data);
        if(!json_data.is_object())
        {
            throw std::runtime_error("error params is not json object");
        }

        Logic logic;
        std::string txid;
        bool ret = logic.AssetDefinition(json_data,txid);
        if(!ret)
        {
            std::string error_info;
            logic.GetErrorInfo(error_info);
            throw std::runtime_error(error_info);
        }

        json json_response;
        json_response["code"] = 0;
        json_response["data"] = txid;
        std::string response = json_response.dump();
        req->WriteHeader("Content-Type", "application/json");
        req->WriteReply(HTTP_OK,response);
        return ;
    }
    catch(json::exception& e)
    {
        json_error["code"] = 1;
        json_error["data"] = e.what();
    }
    catch(std::exception& e)
    {
        json_error["code"] = 10;
        json_error["data"] = e.what();
    }

    replyError(req,json_error);
}

void PushUtxo(std::unique_ptr<HTTPRequest> req)
{

    json json_error;
    try
    {
        std::string post_data = req->ReadBody();
        LOG(INFO) << "Push  utxo:" << post_data;

        json json_data = json::parse(post_data);
        if(!json_data.is_object())
        {
            throw std::runtime_error("error params is not json object");
        }

        Logic logic;
        json json_utxo;
        bool ret = logic.PushUtxo(json_data,json_utxo);

        if(!ret)
        {
            std::string error_info;
            logic.GetErrorInfo(error_info);
            throw std::runtime_error(error_info);
        }

        json json_response;
        json_response["code"] = 0;
        json_response["data"] = json_utxo;
        std::string response = json_response.dump();
        req->WriteHeader("Content-Type", "application/json");
        req->WriteReply(HTTP_OK,response);
        return ;
    }
    catch(json::exception& e)
    {
        json_error["code"] = 1;
        json_error["data"] = e.what();
    }
    catch(std::exception& e)
    {
        json_error["code"] = 10;
        json_error["data"] = e.what();
    }

    replyError(req,json_error);

}

void SendTransaction(std::unique_ptr<HTTPRequest> req)
{

    json json_error;
    try
    {
        std::string post_data = req->ReadBody();
        LOG(INFO) << "Send transaction:" << post_data;

        json json_data = json::parse(post_data);
        if(!json_data.is_object())
        {
            throw std::runtime_error("error params is not json object");
        }

        Logic logic;
        std::string txid;
        bool ret = logic.SendTransaction(json_data,txid);

        if(!ret)
        {
            std::string error_info;
            logic.GetErrorInfo(error_info);
            throw std::runtime_error(error_info);
        }

        json json_response;
        json_response["code"] = 0;
        json_response["data"] = txid;
        std::string response = json_response.dump();
        req->WriteHeader("Content-Type", "application/json");
        req->WriteReply(HTTP_OK,response);
        return ;
    }
    catch(json::exception& e)
    {
        json_error["code"] = 1;
        json_error["data"] = e.what();
    }
    catch(std::exception& e)
    {
        json_error["code"] = 10;
        json_error["data"] = e.what();
    }

    replyError(req,json_error);
}

void GetUtxo(std::unique_ptr<HTTPRequest> req)
{
    json json_error;
    try
    {
        std::string post_data = req->ReadBody();
        LOG(INFO) << "Get Utxo:" << post_data;

        json json_data = json::parse(post_data);
        if(!json_data.is_object())
        {
            throw std::runtime_error("error params is not json object");
        }

        Logic logic;
        json json_utxo = json::array();
        bool ret = logic.GetUtxo(json_data,json_utxo);

        if(!ret)
        {
            std::string error_info;
            logic.GetErrorInfo(error_info);
            throw std::runtime_error(error_info);
        }

        json json_response;
        json_response["code"] = 0;
        json_response["data"] = json_utxo;
        std::string response = json_response.dump();
        req->WriteHeader("Content-Type", "application/json");
        req->WriteReply(HTTP_OK,response);
        return ;
    }
    catch(json::exception& e)
    {
        json_error["code"] = 1;
        json_error["data"] = e.what();
    }
    catch(std::exception& e)
    {
        json_error["code"] = 10;
        json_error["data"] = e.what();
    }

    replyError(req,json_error);
}
