
#include "handler.h"
#include "common/util.h"
#include "data/db_mysql.h"
#include "contract/contract.h"

#include <glog/logging.h>
#include <map>
#include <stdlib.h>
#include "init.h"
#define PRE_LENGHT  8 // precise length
#define FIXED_FEE  0.00001 //fixed  charge fee
#define SETTLE_RECIEVE  0.00005 //fixed  recieve get value

#define  RPC_ERR error_info_ = "Rpc node can't connect!"
#define  CURL_ERR  error_info_ = curl_params_->curl_response

bool  Handler::StructRpcHeader(const std::string rpc_method,json& json_data)
{
    json_data["jsonrpc"] = "1.0";
    json_data["id"] = "curltest";
    json_data["method"] = rpc_method;
    return true;
}

bool Handler::FormatDouble(double& value)
{
    if (value < 0.0)
        return false;
    int64_t l_value  = value;
    double dec_value = value  - l_value;
    char char_value[20];
    gcvt(dec_value,PRE_LENGHT ,char_value);

    value = std::atof(char_value) + (double)l_value;
    return true;

}

static Contract* s_contract  = new Contract();
uint64_t Handler::QueryContract(const json &json_contract_data)
{
    uint64_t token_amout = 0;
    uint64_t token_available = 0;
    uint64_t token_freeze = 0;
    bool ret = QueryBalance(json_contract_data,token_available,token_freeze);
    if(!ret)
        return 0;
    token_amout = token_available + token_freeze;
    return token_amout;
}

bool Handler::IsMyAddress(std::string &address)
{
    if(address.empty())
        return false;

    json json_post;
    StructRpcHeader("validateaddress",json_post);
    json json_params  = json::array();
    json_params.push_back(address);
    json_post["params"] = json_params;
    curl_params_->curl_post_data  = json_post.dump();
    if (!::CurlPost(curl_params_) )
        return false;
    json json_response = json::parse(curl_params_->curl_response);
    json json_result = json_response["result"];
    if (json_result.is_null() ||  json_result["ismine"].is_null())
    {
        LOG(INFO) << curl_params_->curl_post_data ;
        return false;
    };
    bool ret = json_result["ismine"].get<bool>();

    return ret;
}

uint64_t Handler::GetAvailableTokens(const json &json_contract_data)
{
    uint64_t token_available = 0;
    uint64_t token_freeze = 0;
    bool ret = QueryBalance(json_contract_data,token_available,token_freeze);
    if(!ret)
        return 0;
    return token_available;
}

bool Handler::EncodeAddress(const std::string &address, std::string &bch_address)
{
    json json_post;
    json json_params = json::array();
    StructRpcHeader("validateaddress",json_post);
    json_params.clear();
    json_params.push_back(address);
    json_post["params"] = json_params;
    curl_params_->curl_post_data  = json_post.dump();
    if (!::CurlPost(curl_params_))
        return false;
    json json_response = json::parse(curl_params_->curl_response);
    if (json_response["result"].is_null())
    {
        LOG(INFO) << curl_params_->curl_response ;
        return false;
    }

    json result = json_response["result"];
    bch_address = result["address"].get<std::string>();
    return true;
}

bool Handler::CreateTransaction(Tx *tx, std::string &txid)
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

        if (!::CurlPost(curl_params_))
        {
            RPC_ERR;
            return false;
        }

        json_response = json::parse(curl_params_->curl_response);
        if (json_response["result"].is_null())
        {
            LOG(INFO)  << curl_params_->curl_post_data;
            LOG(ERROR) << curl_params_->curl_response;
            error_info_ = curl_params_->curl_response;
            return false;
        }

        json_result = json_response["result"];
        if (json_result.size() <= 0)
        {
            LOG(INFO)  << "has not UTXO : " <<curl_params_->curl_post_data;
            error_info_ = vect_address_input.at(i) + " has not UTXO !" ;
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
            error_info_  = vect_address_input.at(i) + " has not enough money";
            LOG(ERROR) << error_info_ ;
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
    if (!::CurlPost(curl_params_))
    {
        RPC_ERR;
        return false;
    }

    json_response = json::parse(curl_params_->curl_response);

    if (json_response["result"].is_null())
    {
        LOG(INFO) << curl_params_->curl_post_data ;
        LOG(ERROR) << curl_params_->curl_response ;
        CURL_ERR;
        return false;
    }

    std::string tx_hex = json_response["result"].get<std::string>();
    StructRpcHeader("signrawtransaction",json_post);
    json_params.clear();
    json_params.push_back(tx_hex);
    json_post["params"] = json_params;
    curl_params_->curl_post_data  = json_post.dump();
    if (!CurlPost(curl_params_))
    {
        RPC_ERR;
        return false;
    }

    json_response = json::parse(curl_params_->curl_response);
    if(json_response.find("result") == json_response.end())
    {
        LOG(INFO) << curl_params_->curl_post_data ;
        LOG(ERROR) << curl_params_->curl_response ;
        CURL_ERR;
        return false;
    }
    if (json_response["result"].is_null())
    {
        LOG(INFO) << curl_params_->curl_post_data ;
        LOG(ERROR) << curl_params_->curl_response ;
        CURL_ERR;
        return false;
    }
    json_result = json_response["result"];
    std::string sign_tx_hex = json_result["hex"].get<std::string>();

    StructRpcHeader("sendrawtransaction",json_post);
    json_params.clear();
    json_params.push_back(sign_tx_hex);
    json_post["params"] = json_params;
    curl_params_->curl_post_data  = json_post.dump();
    if (!CurlPost(curl_params_))
    {
        RPC_ERR;
        return false;
    }
    json_response = json::parse(curl_params_->curl_response);
    if (json_response["result"].is_null())
    {
        LOG(INFO) << curl_params_->curl_post_data ;
        LOG(ERROR) << curl_params_->curl_response ;
        CURL_ERR;
        return false;
    }
    txid = json_response["result"].get<std::string>();

    return true;
}

void Handler::FlushToDB(WalletInfo* wallet_info)
{
    std::string sql_insert = "INSERT INTO `wallet_info` (`contract`,`asset_id`,`qty`,`address`,`txid`,`type`) VALUES ('";
    // process regtest afddress
    if(wallet_info->address.size() == 49)
    {
        wallet_info->address = wallet_info->address.substr(7);
    }

    if(wallet_info->contract.size() == 49)
    {
        wallet_info->contract = wallet_info->contract.substr(7);
    }

    std::string sql_value = wallet_info->contract + "','" + wallet_info->asset_id+ "','" 
        + std::to_string(wallet_info->qty) + "','"+ wallet_info->address + "','"
        + wallet_info->txid + "','" + std::to_string(wallet_info->type) + "');";
    sql_insert += sql_value;
    LOG(INFO) << "flush wallet: "<<  sql_insert << std::endl;
    g_db_mysql->QuerySql(sql_insert);
}

bool Handler::GetUtxo(const json &json_request, json &json_utxo)
{

    std::string address = json_request["address"].get<std::string>();
    std::string value = json_request["value"].get<std::string>();
    double amount = std::atof(value.c_str());
    std::string bch_address;
    EncodeAddress(address,bch_address);

    std::string select_sql = "select txid,vout,value from utxo where address = '" + bch_address +"' and coinbase = 0;";
    DBMysql::JsonDataFormat json_format;
    json_format.column_size = 3;
    json_format.map_column_type[0] = DBMysql::STRING;
    json_format.map_column_type[1] = DBMysql::INT;
    json_format.map_column_type[2] = DBMysql::STRING;
    json json_data = json::array();
    g_db_mysql->GetDataAsJson(select_sql, &json_format, json_data);
    double total_amount = 0;
    bool  enough = false;
    for (unsigned int i = 0; i < json_data.size(); i++)
    {
        json json_value = json_data.at(i);
        std::string value_tmp = json_value.at(2).get<std::string>();
        double amount_tmp = std::atof(value_tmp.c_str());
        total_amount += amount_tmp;
        if (total_amount >= amount)
        {
           json_utxo.push_back(json_value);
           enough = true;
           break;
        }
        json_utxo.push_back(json_value);
    }

    return enough;
}

void Handler::GetErrorInfo(std::string &error_info)
{
    error_info = error_info_;
}

bool Handler::QueryBalance(const json& json_contract_data, uint64_t& token_amount, uint64_t& freeze_amount)
{
    std::string address = json_contract_data["address"].get<std::string>();
    std::string asset_id = json_contract_data["asset_id"].get<std::string>();
    std::string contract = json_contract_data["contract"].get<std::string>();
    if(address.size() == 49)
    {
        address = address.substr(7);
    }

    if(contract.size() == 49)
    {
        contract = contract.substr(7);
    }

    DBMysql::JsonDataFormat json_format;
    json_format.column_size = 2;
    json_format.map_column_type[0] = DBMysql::INT;
    json_format.map_column_type[1] = DBMysql::INT;
    std::string sql_select = "SELECT `qty`, `id` FROM wallet_info WHERE address = '" + address 
        + "' AND asset_id = '" + asset_id + "' AND contract = '" + contract 
        + "' AND type = " + std::to_string(ActionObject::T4)
        + " ORDER BY id DESC LIMIT 1;";

    json json_data;
    bool ret = g_db_mysql->GetDataAsJson(sql_select,&json_format,json_data);
    if(!ret)
    {
        LOG(ERROR) << "select T4 faild .";
        return false;
    }

    if(json_data.size())
    {
        uint64_t T4_amount = json_data[0][0].get<uint64_t>();
        uint64_t T4_id = json_data[0][1].get<uint64_t>();

        sql_select.clear();
        sql_select = "SELECT `qty` FROM wallet_info WHERE address = '" + address 
            + "' AND asset_id = '" + asset_id + "' AND contract = '" + contract 
            + "' AND type = " + std::to_string(ActionObject::T2)
            + " AND id > " + std::to_string(T4_id)
            + " ORDER BY id DESC LIMIT 1;";

        json_format.column_size = 1;
        json_format.map_column_type.clear();
        json_format.map_column_type[0] = DBMysql::INT;
        json_data.clear();
        ret = g_db_mysql->GetDataAsJson(sql_select,&json_format,json_data);
        if(!ret)
        {
            LOG(ERROR) << "select T2 > T4 faild .";
            return false;
        }

        if(json_data.size()) 
        {
            uint64_t T2_amount = json_data[0][0].get<uint64_t>();
            token_amount = T4_amount - T2_amount;
            freeze_amount = T2_amount;
            LOG(INFO) << asset_id << " asset token amount : " << token_amount
                << ", and freeze amount : " << freeze_amount;
        }
        else
        {
            token_amount = T4_amount; 
            LOG(INFO) << asset_id << " asset token amount : " << token_amount;
        }
    }
    else
    {
        sql_select.clear();
        sql_select = "SELECT `qty` FROM wallet_info WHERE address = '" + address 
             + "' AND asset_id = '" + asset_id + "' AND contract = '" + contract 
             + "' AND type = " + std::to_string(ActionObject::A2);

        json_format.column_size = 1;
        json_format.map_column_type.clear();
        json_format.map_column_type[0] = DBMysql::INT;
        json json_data;
        ret = g_db_mysql->GetDataAsJson(sql_select,&json_format,json_data);
        if(!ret)
        {
            LOG(ERROR) << "select A2 faild .";
            return false;
        }

        if(json_data.size() == 0)
        {
            LOG(INFO) << asset_id << " asset not creation.";
            return true;
        }

        uint64_t A2_amount = json_data[0][0].get<uint64_t>();

        sql_select.clear();
        sql_select = "SELECT `qty` FROM wallet_info WHERE address = '" + address 
            + "' AND asset_id = '" + asset_id + "' AND contract = '" + contract 
            + "' AND type = " + std::to_string(ActionObject::T2)
            + " ORDER BY id DESC LIMIT 1;";

        json_format.column_size = 1;
        json_format.map_column_type.clear();
        json_format.map_column_type[0] = DBMysql::INT;
        json_data.clear();
        ret = g_db_mysql->GetDataAsJson(sql_select,&json_format,json_data);
        if(!ret)
        {
            LOG(ERROR) << "select T2 faild .";
            return false;
        }
        if(json_data.size())
        {
            uint64_t T2_amount = json_data[0][0].get<uint64_t>();
            token_amount = A2_amount - T2_amount;
            freeze_amount = T2_amount;
            LOG(INFO) << asset_id << " asset token amount : " << token_amount
                << ", and freeze amount : " << freeze_amount;
        }
        else
        {
            token_amount = A2_amount;
            LOG(INFO) << asset_id << " asset token amount : " << token_amount;
        }
    }
    return true;
}

bool Handler::SendTransaction(const json& json_contract_data,std::string &txid)
{
    // TODO 广播前需要解析并验证
    std::string singed_tx_hex = json_contract_data["hex"].get<std::string>();
    json json_post;
    json json_params = json::array();
    StructRpcHeader("sendrawtransaction",json_post);
    json_params.clear();
    json_params.push_back(singed_tx_hex);
    json_post["params"] = json_params;
    curl_params_->curl_post_data  = json_post.dump();
    if (!CurlPost(curl_params_))
        return false;
    json json_response = json::parse(curl_params_->curl_response);
    if (json_response["result"].is_null())
    {
        LOG(INFO) << curl_params_->curl_response ;
        CURL_ERR;
        return false;
    }
    txid = json_response["result"].get<std::string>();
    json json_utxo = json_contract_data["utxo"];
    if (json_utxo.is_null())
    {
        LOG(INFO) << "has not utxo" << std::endl;
        error_info_ = "Should input utxo";
        return false;
    }
    std::string sql_del;
    for(unsigned int i = 0; i < json_utxo.size(); i++)
    {
        std::string txid = json_utxo[i][0].get<std::string>();
        int vout = json_utxo[i][1].get<int>();
        sql_del = "delete from utxo where txid = '";
        sql_del += txid +"' and vout = " + std::to_string(vout);
        g_db_mysql->QuerySql(sql_del);
    }
    return true;
}



