#include "logickafka.h"
#include "contract/contractobject.h"
#include <glog/logging.h>
#define PRE_LENGHT  8 // precise length
#define FIXED_FEE  0.00001 //fixed  charge fee
#define SETTLE_RECIEVE  0.00005 //fixed  recieve get value

LogicKafka::LogicKafka()
{
    contract_curl_params_ = new token_interactive::CurlParams();
    contract_curl_params_->curl_url = token_init::g_map_conf["contract_node_url"];
    contract_curl_params_->need_auth = false;
}

LogicKafka::~LogicKafka()
{
    delete contract_curl_params_;
}

bool LogicKafka::ExchangeSign(const json &json_contract)
{
    std::string contract_address = json_contract["contract_address"].get<std::string>();
    double utxo_currency = json_contract["utxo_currency"].get<double>();;
    double currency = json_contract["currency"].get<double>();
    json utxo = json_contract["utxo"];
    std::string send_address = json_contract["send_address"];
    std::string recieve_address = json_contract["recieve_address"];
    std::string asset_type = json_contract["asset_type"].get<std::string>();
    std::string asset_id = json_contract["asset_id"].get<std::string>();
    std::string asset_id_hash ;
    ContractObject::GetContractHash(asset_id,asset_id_hash);
    std::cout<<"asset_id :" << asset_id <<std::endl;
    std::cout<<"asset_id_hash :" << asset_id_hash <<std::endl;
    uint64_t shares = json_contract["shares"].get<uint64_t>();

    Contract* contract  = new Contract();
    contract->SetContractType(Contract::SHC_TRANSFER);
    contract->SetContractAddress(contract_address);
    contract->SetBchCountT2(currency);
    contract->SetTokenSendAddress(send_address);
    contract->SetTokenRecieveAddress(recieve_address);

    NCAct action = new Action::TransferOperation();
    NCActT action_t =  (NCActT)action;
    action_t->operation_code = Action::T2;
    action_t->party1_asset_type = asset_type;
    action_t->party1_asset_id = asset_id_hash;
    action_t->party1_token_qty = shares;
    action_t->timestamp = "20181230";
    action_t->exchange_fee_currency = "BCH";

    action_t->exchange_fee_var = 0.2;
    action_t->exchange_fee_fixed = 0.02;
    action_t->exchange_fee_address  = send_address;

    std::string op_ret_data;
    contract->FormatOpReturnData(action,op_ret_data);
    LOG(INFO) << "T2  opreturn: " << op_ret_data;
    Tx* tx = new Tx();

    tx->PushInputAddress(send_address);
    tx->PushOutputAddress(contract_address);
    tx->PushOutputAddress(recieve_address);
    tx->map_input_amout_[send_address] = 0.00005;
    tx->map_input_amout_[recieve_address] = currency;
    tx->map_output_amount_[contract_address] = currency;
    tx->map_output_amount_[send_address] = 0.00005;

    tx->SetOpReturnData(op_ret_data);
    std::string tx_hex;

    CreateUnSignTransaction(tx,utxo,utxo_currency,recieve_address,tx_hex);

    json json_content;
    json_content["hex"] = tx_hex;
    json json_kafka;
    json_kafka["code"] = UNSIGN_TX;
    json_kafka["target"] = recieve_address;
    json_kafka["content"] = json_content;

    WalletInfo* wallet_info = new WalletInfo();
    wallet_info->contract = contract_address;
    wallet_info->asset_id = asset_id_hash;
    wallet_info->address = send_address;
    wallet_info->txid = "utxo";
    wallet_info->type = ActionObject::T2;
    wallet_info->qty = shares;
    FlushToDB(wallet_info);
    delete wallet_info;

    token_init::g_kafka_producer->PushData(json_kafka.dump());

    delete tx;
    delete action;
    delete contract;
    return true;
}


bool LogicKafka::PushSign(const json &json_contract,std::string &txid)
{
    // 签名 
    ContractObject contract_obj;
    std::string tx_hex = json_contract["hex"].get<std::string>();
    std::string priv_key;
    std::string pub_key;
    if(!ContractObject::GenerateEncryptKey(priv_key,pub_key))
    {
        LOG(ERROR)<<"Generate encrypt key falid!";
        return false;
    }
    contract_obj.SetPrivateKey(priv_key);

    std::string sign_contract;
    if(!contract_obj.SignContract(tx_hex,sign_contract))
    {
        LOG(ERROR)<<"Sign contract falid!";
        return false;
    }

    // 调用合约提供的SendTransaction http接口
    json json_post;
    json json_params = json::array();
    StructRpcHeader("SendTransaction",json_post);
    json_params.clear();
    json_params.push_back(sign_contract);
    json_post["params"] = json_params;
    contract_curl_params_->curl_post_data  = json_post.dump();
    if (!token_interactive::CurlPost(contract_curl_params_))
        return false;
    json json_response = json::parse(contract_curl_params_->curl_response);
    if (json_response["result"].is_null())
    {
        LOG(INFO) << contract_curl_params_->curl_response ;
        return false;
    }
    txid = json_response["result"].get<std::string>();
 
    return true;
}

// bool LogicKafka::PushSign(const json &json_contract, std::string &txid)
// {
//     std::string tx_hex = json_contract["hex"].get<std::string>();
//     token_interactive::CurlParams* curl_params_ = new token_interactive::CurlParams();
//     curl_params_->curl_url = token_init::g_map_conf["block_node_url"];
//     curl_params_->curl_auth = token_init::g_map_conf["rpc_auth"];
//     curl_params_->need_auth = true;
// 
//     json json_post;
//     json json_params  = json::array();
// 
//     StructRpcHeader("signrawtransaction",json_post);
//     json_params.clear();
//     json_params.push_back(tx_hex);
//     json_post["params"] = json_params;
//     curl_params_->curl_post_data  = json_post.dump();
//     if (!token_interactive::CurlPost(curl_params_))
//         return false;
// 
//     json json_response = json::parse(curl_params_->curl_response);
//     if(json_response.find("result") == json_response.end())
//     {
//         return false;
//     }
//     if (json_response["result"].is_null())
//     {
//         LOG(INFO) << curl_params_->curl_response ;
//         return false;
//     }
//     json json_result = json_response["result"];
//     std::string sign_tx_hex = json_result["hex"].get<std::string>();
// 
//     StructRpcHeader("sendrawtransaction",json_post);
//     json_params.clear();
//     json_params.push_back(sign_tx_hex);
//     json_post["params"] = json_params;
//     curl_params_->curl_post_data  = json_post.dump();
//     if (!token_interactive::CurlPost(curl_params_))
//         return false;
//     json_response = json::parse(curl_params_->curl_response);
//     if (json_response["result"].is_null())
//     {
//         LOG(INFO) << curl_params_->curl_response ;
//         return false;
//     }
//     txid = json_response["result"].get<std::string>();
//     return true;
// }

bool LogicKafka::CreateUnSignTransaction(Tx *tx, json &utxo, double utxo_currency, const std::string &utxo_address, std::string &tx_hex)
{

    std::vector<std::string> vect_address_input;
    std::vector<std::string> vect_address_output;
    tx->GetInputAddress(vect_address_input);
    tx->GetOutputAddress(vect_address_output);

    json json_post;
    json json_params  = json::array();
    json json_address_array = json::array();
    json json_response;
    json json_result;
    json json_txid_array= json::array();
    std::map<std::string,double> map_charge_fee;

    for(unsigned int i = 0; i < vect_address_input.size(); i++)
    {
        StructRpcHeader("listunspent",json_post);
        json_address_array.clear();
        json_address_array.push_back(vect_address_input.at(i));
        json_params.clear();
        json_params.push_back(0);
        json_params.push_back(9999999);
        json_params.push_back(json_address_array);
        json_post["params"] = json_params;
        curl_params_->curl_post_data  = json_post.dump();

        if (!token_interactive::CurlPost(curl_params_) )
            return false;

        json_response = json::parse(curl_params_->curl_response);
        if (json_response["result"].is_null())
        {
            LOG(INFO) << curl_params_->curl_response;
           return false;
        }

        json_result = json_response["result"];
        if (json_result.size() <= 0)
        {
            return false;
        }
        //first address pays the charge fee .
        double pay_amout = tx->map_input_amout_[vect_address_input.at(i)];
        if ( i == 0)
            pay_amout += FIXED_FEE;
        double add_amout = 0.0;
        for (unsigned int j = 0; j < json_result.size(); j++ )
        {
            json json_value  = json_result.at(j);
            std::string txid_spent = json_value["txid"].get<std::string>();
            int vout = json_value["vout"].get<int>();
            double amount = json_value["amount"].get<double>();
            add_amout += amount;
            json json_txid;
            json_txid["txid"] = txid_spent;
            json_txid["vout"] = vout;
            json_txid_array.push_back(json_txid);
            if (add_amout > pay_amout )
            {
                //settle  the charge  fee
                double charge_fee = add_amout - pay_amout;
                FormatDouble(charge_fee);
                map_charge_fee[vect_address_input.at(i)] = charge_fee;
                break;
            }
        }

        if (pay_amout  > add_amout)
        {
            std::string error_info  = vect_address_input.at(i) + " has not enough money";
            LOG(INFO) << error_info ;
            return false;

        }
    }

    std::string hex_data ;
    tx->GetOpReturnData(hex_data);

    json json_tx_output;
    json_tx_output["data"] = hex_data;
    std::map<std::string,double>::const_iterator cont_it = tx->map_output_amount_.begin();
    double out_put_fee = 0.0;
    //if charge address is same as output address, merge amount
    for (;cont_it != tx->map_output_amount_.end(); cont_it++ )
    {
        out_put_fee += cont_it->second;
        if (map_charge_fee.find(cont_it->first) != map_charge_fee.end())
        {
            json_tx_output[cont_it->first] =  cont_it->second + map_charge_fee[cont_it->first];
            map_charge_fee.erase(cont_it->first);
        }
        else
        {
            json_tx_output[cont_it->first] = cont_it->second;
        }
    }

    for (unsigned int i = 0; i < utxo.size();  i++)
    {
        json_txid_array.push_back(utxo.at(i));
    }
    json_tx_output[utxo_address] = utxo_currency - tx->map_input_amout_[utxo_address];

    //put charge fee to output
    cont_it = map_charge_fee.begin();
    for (;cont_it != map_charge_fee.end(); cont_it++)
    {
        json_tx_output[cont_it->first] = cont_it->second;
    }

    json_params.clear();
    json_params.push_back(json_txid_array);
    json_params.push_back(json_tx_output);

    StructRpcHeader("createrawtransaction",json_post);
    json_post["params"] = json_params;

    curl_params_->curl_post_data  = json_post.dump();
    if (!token_interactive::CurlPost(curl_params_))
        return false;

    json_response = json::parse(curl_params_->curl_response);
    if (json_response["result"].is_null())
    {
        LOG(INFO) << curl_params_->curl_post_data;
        LOG(ERROR) << curl_params_->curl_response ;
        return false;
    }

    tx_hex = json_response["result"].get<std::string>();
    StructRpcHeader("signrawtransaction",json_post);
    json_params.clear();
    json_params.push_back(tx_hex);
    json_post["params"] = json_params;
    curl_params_->curl_post_data  = json_post.dump();
    if (!token_interactive::CurlPost(curl_params_))
        return false;

    json_response = json::parse(curl_params_->curl_response);
    if (json_response["result"].is_null())
    {
        LOG(INFO) << curl_params_->curl_post_data;
        LOG(ERROR) << curl_params_->curl_response ;
        return false;
    }
    json_result = json_response["result"];
    tx_hex = json_result["hex"].get<std::string>();
    return true;
}
