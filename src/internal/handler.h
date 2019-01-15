#ifndef HANDLER_H
#define HANDLER_H
#include <string>
#include <vector>
#include "common/json.hpp"
#include "contract/designcontract.h"
#include "contract/contract.h"
#include "contract/contractobject.h"
#include "transaction/tx.h"
#include "transaction/txobject.h"
#include "common/util.h"
#include "contract_machine/action.h"
#include "init.h"

using json = nlohmann::json;

class Handler
{
public:
    struct WalletInfo
    {
        std::string contract;
        std::string asset_id;
        std::string address;
        std::string txid;
        ActionObject::ContractCode type;
        uint64_t qty;
    };

    Handler()
    {
        curl_params_ = new CurlParams();
        curl_params_->curl_url = g_map_conf["block_node_url"];
        curl_params_->curl_auth = g_map_conf["rpc_auth"];
        curl_params_->need_auth = true;
    }
    ~Handler()
    {
        delete curl_params_;
    }
public:
    std::vector<std::string> CreateContract(json json_contract_data);
    std::vector<std::string> TransferContract(json json_contract_data);
    uint64_t QueryContract(const json& json_contract_data);

public:

    bool PushUtxo(const json& json_contract, json &utxo);

    bool IsMyAddress(std::string& address);

    uint64_t GetAvailableTokens(const json &json_contract_data);

    void FlushToDB(WalletInfo* wallet_info);

    bool GetUtxo(const json& json_request, json& json_utxo);

public:
    bool SendTransaction(const json& json_contract_data,std::string &txid);

public:
    void GetErrorInfo(std::string& error_info);

protected:

    bool EncodeAddress(const std::string& address, std::string& bch_address);

    bool CreateTransaction(Tx*tx, std::string& txid);

    bool StructRpcHeader(const std::string rpc_method,json& json_data);

    bool FormatDouble(double& value);

    bool QueryBalance(const json& json_contract_data, uint64_t& token_amount, uint64_t& freeze_amount);

protected:
    CurlParams* curl_params_;
    std::string error_info_;

};

#endif
