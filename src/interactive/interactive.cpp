#include "interactive.h"
#include "data/db_mysql.h"
#include <glog/logging.h>

static bool StructRpcHeader(const std::string rpc_method,json& json_data)
{
    json_data["jsonrpc"] = "1.0";
    json_data["id"] = "curltest";
    json_data["method"] = rpc_method;
    return true;
}

Interative::Interative()
{
    rpc_url_.clear();
    sdk_url_.clear();
    access_block_chain_type_ = BLOCK_CHAIN_RPC;
    curl_param_ = new token_interactive::CurlParams();
}

Interative::~Interative()
{
    delete curl_param_;
}

void Interative::CreateNewConstractAddress(std::string &contract_address)
{
    switch (access_block_chain_type_)
    {
    case BLOCK_CHAIN_RPC:
        GetRpcGetNewAddress(contract_address);
        break;
    case SDK_INTERFACE:
        GetSdkGetNewAddress(contract_address);
        break;
    default:
        break;
    }

}

void Interative::SetBlockChainRpcUrl(const std::string &rpc_url)
{
    rpc_url_ = rpc_url;
}

void Interative::SetSdkUrl(const std::string &sdk_url)
{
    sdk_url_ = sdk_url;
}

void Interative::SetAccessBlockChainType(Interative::AccessBlockChainType access_type)
{
    access_block_chain_type_  = access_type;
}

void Interative::SetRpcAuth(const std::string &rpc_auth)
{
    rpc_auth_ = rpc_auth;
}

void Interative::GetRpcGetNewAddress(std::string &new_address)
{
    curl_param_->curl_url = rpc_url_;
    curl_param_->curl_auth = rpc_auth_;
    curl_param_->need_auth = true;

    json json_post;
    json json_params = json::array();
    StructRpcHeader("getnewaddress",json_post);
    json_post["params"] = json_params;

    curl_param_->curl_post_data = json_post.dump();
    if ( !token_interactive::CurlPost(curl_param_) )
    {
        LOG(ERROR) << "RPC getnewaddress failed ";
    }
    else
    {
        json json_response  = json::parse(curl_param_->curl_response);
        if (!json_response["error"].is_null())
        {
            LOG(ERROR) << "RPC getnewaddress  error : " << curl_param_->curl_response;
        }
        else
        {
            new_address = json_response["result"].get<std::string>();
        }
    }

}

void Interative::GetSdkGetNewAddress(std::string &new_address)
{

}
