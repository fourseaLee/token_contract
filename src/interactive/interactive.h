#ifndef INTERACTIVE_H
#define INTERACTIVE_H
#include <string>
#include "tokeninteractive.h"

class Interative
{
public:
    Interative();
    ~Interative();
    enum AccessBlockChainType
    {
        BLOCK_CHAIN_RPC = 1,
        SDK_INTERFACE  = 2
    };

public:

    void CreateNewConstractAddress(std::string& contract_address);

    void SetBlockChainRpcUrl(const std::string& rpc_url);

    void SetSdkUrl(const std::string& sdk_url);

    void SetAccessBlockChainType(AccessBlockChainType access_type);

    void SetRpcAuth(const std::string& rpc_auth);

protected:
    void GetRpcGetNewAddress(std::string& new_address);

    void GetSdkGetNewAddress(std::string& new_address);
public:

protected:
    int access_block_chain_type_;
    std::string rpc_url_;
    std::string rpc_auth_;
    std::string sdk_url_;
    token_interactive::CurlParams* curl_param_;

};

#endif // INTERACTIVE_H
