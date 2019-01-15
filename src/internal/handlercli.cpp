#include "handlercli.h"
#include  "common/json.hpp"
#include <map>
using json = nlohmann::json;
/*
typedef void (*PF)(const std::string& params,std::string& result);
static  std::map<std::string,PF> s_map_function;
*/

CurlParams* s_curl_params = new CurlParams();
#define UTIL_TEST

static Contract* s_contract  = new Contract();

HandlerCli::HandlerCli()
{

}

void HandlerCli::SetContractUrl(const std::string &contract_url)
{
    contract_url_ = contract_url;
    s_curl_params->curl_url = contract_url_;
    s_curl_params->need_auth = true;
}

void HandlerCli::GetContractUrl(std::string &contract_url)
{
    contract_url = contract_url_;
}

void HandlerCli::RegisterFunction()
{

}

bool HandlerCli::RunFunction(const std::string &function, const std::string &params, std::string& result)
{

    if (function == "Help")
    {
        return Help(params,result);
    }
    else if(function == "GetUtxo")
    {
        return GetUtxo(params,result);
    }
    else if(function == "ContractOffer")
    {
        return ContractOffer(params,result);
    }
    else if(function == "AssetDefinition")
    {
        return AssetDefinition(params,result);
    }
    else if(function == "TokenSend")
    {
        return TokenSend(params,result);
    }
    else if(function == "TokenRecieve")
    {
        return TokenRecieve(params,result);
    }
    else if(function == "GetBalance")
    {
        return GetBalance(params,result);
    }

    result = function + "is not found!";
    return true;
}

bool HandlerCli::Help(const std::string &params, std::string &result)
{
    std::cout << "client  help info: " << std::endl;
    return true;
}

bool HandlerCli::GetUtxo(const std::string &params, std::string &result)
{
    std::cout << "GetUtxo: " << params << std::endl;
    json json_params = json::parse(params);
    std::string address = json_params["address"].get<std::string>();
    std::string value = json_params["value"].get<std::string>();
    s_curl_params->curl_url += "/GetUtxo";
    json json_post;
    json_post["address"] = address;
    json_post["value"] = value;
    s_curl_params->curl_post_data = json_post.dump();

    if (!CurlPost(s_curl_params))
    {
        std::cout << "GetUtxo failed!" << std::endl;
        return false;
    }

    json json_response = json::parse(s_curl_params->curl_response);
    json json_result  = json_response["data"];
    s_curl_params->curl_url = contract_url_;
    result = json_result.dump();
    return true;
}

bool HandlerCli::ContractOffer(const std::string &params, std::string &result)
{
    std::cout << "ContractOffer: " << params << std::endl;
    json json_params = json::parse(params);
    std::string issuer_address = json_params["issuer_address"].get<std::string>();
    std::string contract_address = json_params["contract_address"].get<std::string>();
    std::string contract_name = json_params["contract_name"].get<std::string>();
    std::string contract_content = json_params["contract_content"].get<std::string>();
    double contract_fee = json_params["contract_fee"].get<double>();
    double miner_fee = json_params["miner_fee"].get<double>();
    std::string priv_key_base58 = json_params["priv_key_base58"].get<std::string>();
    json json_utxo = json_params["utxo"];
    std::cout <<  "issuer: " << issuer_address << std::endl;
    std::cout <<  "contract: " << contract_address << std::endl;

    NCAct  action = new Action::ContractOperation();
    NCActC action_c =  (NCActC)action;
    action_c->operation_code = Action::C1;
    action_c->contract_name = contract_name;
    action_c->contract_file_hash = contract_content;
    std::string op_ret_data;

    bool ret = s_contract->FormatOpReturnData(action,op_ret_data);
    if(!ret)
    {
        result = "serialize action C1 failed!";
        delete action;
        return ret;
    }
    delete action;
    std::string tx_hex;
    BuildTransaction(priv_key_base58,op_ret_data,json_utxo,
                     issuer_address,contract_address,contract_fee,miner_fee,tx_hex);
    std::string txid ;
    SendRawtransaction(tx_hex,json_utxo,txid);
    result = txid;
    return true;
}

bool HandlerCli::AssetDefinition(const std::string &params, std::string &result)
{
    std::cout << "AssetDefinition: " <<  std::endl;
    json json_params = json::parse(params);
    std::string issuer_address = json_params["issuer_address"].get<std::string>();
    std::string contract_address = json_params["contract_address"].get<std::string>();
    std::string asset_id = json_params["asset_id"].get<std::string>();
    std::string asset_id_hash ;
    ContractObject::GetContractHash(asset_id,asset_id_hash);
    uint64_t qty = json_params["qty"].get<uint64_t>();
    double contract_fee = json_params["contract_fee"].get<double>();
    double miner_fee = json_params["miner_fee"].get<double>();
    std::string priv_key_base58 = json_params["priv_key_base58"].get<std::string>();
    json json_utxo = json_params["utxo"];

    NCAct action = new Action::AssetOperation();
    NCActA action_a = (NCActA)action;
    action_a->operation_code = Action::A1;
    action_a->asset_type =  "SHC";//json_contract->asset_type;
    action_a->asset_id = asset_id_hash;
    action_a->qty = qty;
    action_a->contract_fee_currency = "BCH";
    action_a->contract_fee_var = 0.01;
    action_a->contract_fee_fixed = 15;

    std::string op_ret_data;
    bool ret = s_contract->FormatOpReturnData(action,op_ret_data);
    if(!ret)
    {
        result = "serialize action A1 failed!";
        delete action;
        return ret;
    }

    delete action;
    std::string tx_hex;
    BuildTransaction(priv_key_base58,op_ret_data,json_utxo,
                     issuer_address,contract_address,
                     contract_fee,miner_fee,tx_hex);

    SendRawtransaction(tx_hex,json_utxo,result);

    return true;
}

bool HandlerCli::TokenSend(const std::string &params, std::string &result)
{
    std::cout << "TokenSend: " << std::endl;
    json json_params = json::parse(params);
    std::string contract_address = json_params["contract_address"].get<std::string>();
    double currency = json_params["currency"].get<double>();
    double contract_fee = json_params["contract_fee"].get<double>();
    double miner_fee = json_params["miner_fee"].get<double>();
    json utxo_send = json_params["utxo_send"];
    json utxo_recieve = json_params["utxo_recieve"];
    std::string priv_key_base58 = json_params["priv_key_base58"].get<std::string>(); ;
    std::string send_address = json_params["send_address"].get<std::string>();
    std::string send_serialize_address = json_params["send_serialize_address"].get<std::string>();
    std::string recieve_address = json_params["recieve_address"];
    std::string asset_type = json_params["asset_type"].get<std::string>();
    std::string asset_id = json_params["asset_id"].get<std::string>();
    std::string asset_id_hash ;
    ContractObject::GetContractHash(asset_id,asset_id_hash);
    std::cout<<"asset_id :" << asset_id <<std::endl;
    std::cout<<"asset_id_hash :" << asset_id_hash <<std::endl;
    uint64_t shares = json_params["shares"].get<uint64_t>();

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
    action_t->exchange_fee_address  = send_serialize_address;
    std::string op_ret_data;
    bool ret = s_contract->FormatOpReturnData(action,op_ret_data);
    delete action;

    ContractObject::BuildTx build_tx;
    build_tx.sign_all = false;
    build_tx.opreturn_data =op_ret_data;
    build_tx.priv_key_base58 = priv_key_base58;
    double send_utxo_amount = 0.0;
    int sign_pos = 0;
    for (unsigned int i = 0; i <utxo_send.size(); i++)
    {
        json json_value = utxo_send.at(i);
        ContractObject::BuildTxVin build_vin;
        build_vin.txid = json_value.at(0).get<std::string>();
        build_vin.vin_index = json_value.at(1).get<int>();
        build_vin.value = json_value.at(2).get<std::string>();
        send_utxo_amount += std::atof(build_vin.value.c_str());
        build_tx.vect_vin.push_back(build_vin);
        build_tx.vect_pos.push_back(sign_pos);
        sign_pos ++;

    }

    double recieve_utxo_amount = 0.0;
    for (unsigned int i = 0; i < utxo_recieve.size(); i++)
    {
        json json_value = utxo_recieve.at(i);
        ContractObject::BuildTxVin build_vin;
        build_vin.txid = json_value.at(0).get<std::string>();
        build_vin.vin_index = json_value.at(1).get<int>();
        build_vin.value = json_value.at(2).get<std::string>();
        recieve_utxo_amount += std::atof(build_vin.value.c_str());
        build_tx.vect_vin.push_back(build_vin);
    }

    build_tx.map_vout[contract_address] = std::to_string(currency + contract_fee);
    build_tx.map_vout[send_address] = std::to_string(send_utxo_amount  - miner_fee);//sender  pay miner fee
    build_tx.map_vout[recieve_address] = std::to_string(recieve_utxo_amount - currency -contract_fee);
    std::string tx_hex;
    s_contract->CreateTransaction(build_tx,tx_hex);
    std::string txid ;
   // SendRawtransaction(tx_hex, txid);
    result = txid;
    return true;
}

bool HandlerCli::TokenRecieve(const std::string &params, std::string &result)
{
    std::cout << "TokenRecieve: " << std::endl;

    json json_params = json::parse(params);

    std::string priv_key_base58 = json_params["priv_key_base58"];
    std::string tx_hex = json_params["tx_hex"];
    json json_utxo = json_params["utxo"];
    std::string tx_result;
    std::vector<ContractObject::BuildTxVin> vect_vin;
    for (unsigned int i = 0; i < json_utxo.size(); i++)
    {
        json json_value = json_utxo.at(i);
        ContractObject::BuildTxVin build_vin;
        build_vin.txid = json_value.at(0).get<std::string>();
        build_vin.vin_index = json_value.at(1).get<int>();
        build_vin.value = json_value.at(2).get<std::string>();
        vect_vin.push_back(build_vin);
    }
    s_contract->SignTransaction(priv_key_base58,tx_hex,vect_vin,tx_result);
    std::string txid ;
    SendRawtransaction(tx_result,json_utxo,txid);
    result = txid;

    return true;
}

bool HandlerCli::GetBalance(const std::string &params, std::string &result)
{
    std::cout << "GetBalance: " << std::endl;
    return true;
}

bool HandlerCli::BuildTransaction(const std::string&priv_key_base58,
                                const std::string& op_ret_data,
                                const json&json_utxo,
                                const std::string& issuer_address,
                                const std::string& contract_address,
                                double contract_fee,
                                double miner_fee ,
                                std::string& tx_hex)
{

    ContractObject::BuildTx build_tx;
    build_tx.opreturn_data =op_ret_data;
    build_tx.priv_key_base58 = priv_key_base58;
    double utxo_amount = 0.0;
    for (unsigned int i = 0; i < json_utxo.size(); i++)
    {
        json json_value = json_utxo.at(i);
        ContractObject::BuildTxVin build_vin;
        build_vin.txid = json_value.at(0).get<std::string>();
        build_vin.vin_index = json_value.at(1).get<int>();
        build_vin.value = json_value.at(2).get<std::string>();
        utxo_amount += std::atof(build_vin.value.c_str());
        build_tx.vect_vin.push_back(build_vin);
    }

    build_tx.map_vout[contract_address] = std::to_string(contract_fee);
    build_tx.map_vout[issuer_address] = std::to_string(utxo_amount - contract_fee - miner_fee);
    s_contract->CreateTransaction(build_tx,tx_hex);
    return true;
}

bool HandlerCli::SendRawtransaction(const std::string &tx_hex,const json& json_utxo, std::string &txid)
{

    s_curl_params->curl_url += "/SendTransaction";
    std::cout << "send transaction : " << s_curl_params->curl_url << std::endl;
    json json_post;
    json_post["hex"] = tx_hex;
    json_post["utxo"] = json_utxo;
    s_curl_params->curl_post_data = json_post.dump();

    if (!CurlPost(s_curl_params))
    {
        std::cout << "SendTrasaction failed!" << std::endl;
        return false;
    }
    std::cout << s_curl_params->curl_post_data << std::endl;
    json json_response = json::parse(s_curl_params->curl_response);
    int code = json_response["code"].get<int>();
    if ( 0 != code)
    {
        std::cout << "error :" << s_curl_params->curl_response << std::endl;
        return false;
    }
    txid = json_response["data"].get<std::string>();
    s_curl_params->curl_url = contract_url_;

    std::cout << "send rawtransaction: " << txid << std::endl;
    return true;
}


