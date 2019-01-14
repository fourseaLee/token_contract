#include <string>
#include <vector>
#include "json.hpp"
#include "contract/designcontract.h"
#include "contract/contract.h"
#include "contract/contractobject.h"
#include "transaction/tx.h"
#include "transaction/txobject.h"
#include "interactive/tokeninteractive.h"
#include "contract_machine/action.h"
#include "init.h"

using json = nlohmann::json;

class Logic
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
    enum  KafkaMessageCode
    {
        UTXO = 1,
        UNSIGN_TX = 2,
    };

    Logic()
    {
        contract_ = new DesignContract();
        contract_format_ = new DesignContract::UserContractFormat();
        transfer_ = new DesignContract::TransferSHC();
        curl_params_ = new token_interactive::CurlParams();
        curl_params_->curl_url = token_init::g_map_conf["block_node_url"];
        curl_params_->curl_auth = token_init::g_map_conf["rpc_auth"];
        curl_params_->need_auth = true;
    }
    ~Logic()
    {
        delete contract_;
        delete contract_format_;
        delete transfer_;
        delete curl_params_;
    }
public:
    std::vector<std::string> createContract(json json_contract_data);
    std::vector<std::string> transferContract(json json_contract_data);
    uint64_t queryContract(const json& json_contract_data);

public:
    bool ContractOffer(const json& json_contract, std::string& txid);

    bool AssetDefinition(const json& json_contract, std::string& txid);

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
    bool GetUTXO(const std::string&address, const double& currency, json& utxo, double &utxo_currency);

    bool  EncodeAddress(const std::string& address, std::string& bch_address);

    bool CreateTransaction(Tx*tx, std::string& txid);

    bool StructRpcHeader(const std::string rpc_method,json& json_data);

    bool FormatDouble(double& value);

    bool queryTokens(const json& json_contract_data, uint64_t& token_amount, uint64_t& freeze_amount);

protected:

    token_interactive::CurlParams* curl_params_;
    std::string error_info_;

private:
    DesignContract* contract_;
    DesignContract::UserContractFormat* contract_format_; 
    DesignContract::TransferSHC* transfer_;


};
