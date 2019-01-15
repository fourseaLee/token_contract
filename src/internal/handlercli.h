#ifndef HANDLERCLI_H
#define HANDLERCLI_H
#include "handler.h"
#include <string>



class HandlerCli : public Handler
{
public:
    HandlerCli();

    void SetContractUrl(const std::string& contract_url);

    void GetContractUrl(std::string& contract_url);

    void RegisterFunction();

    bool RunFunction(const std::string& function, const std::string& params, std::string& result);

protected:

    bool Help(const std::string&params,std::string& result);

    bool GetUtxo(const std::string &params, std::string &result);

    bool ContractOffer(const std::string &params, std::string &result);

    bool AssetDefinition(const std::string &params, std::string &result);

    bool TokenSend(const std::string &params, std::string &result);

    bool TokenRecieve(const std::string &params, std::string &result);

    bool GetBalance(const std::string &params, std::string &result);
private:
    bool BuildTransaction(const std::string&priv_key_base58,
                          const std::string& op_ret_data,
                          const json&json_utxo,
                          const std::string& issuer_address,
                          const std::string& contract_address,
                          double contract_fee,
                          double miner_fee ,
                          std::string& tx_hex);

    bool SendRawtransaction(const std::string& tx_hex, const json &json_utxo, std::string& txid);


protected:
    std::string contract_url_;

};

#endif // HANDLERCLI_H
