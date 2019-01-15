#include "parsetx.h"
#include  "data/db_mysql.h"
#include <iostream>
#include <glog/logging.h>
#include "internal/handlerscan.h"

static bool StructRpcHeader(const std::string rpc_method,json& json_data)
{
    json_data["jsonrpc"] = "1.0";
    json_data["id"] = "curltest";
    json_data["method"] = rpc_method;
    return true;
}

static bool IsZero(double value)
{
    if(value <= 0.0)
        return true;
    return false;
}

ParseTx::ParseTx()
{
    curl_param_ = new CurlParams();
}

ParseTx::~ParseTx()
{
    delete curl_param_;
}

void ParseTx::SetFilterType(TxType filer_type)
{
    filter_type_ = filer_type;
}

void ParseTx::SetPreBlockHeight(uint64_t pre_block_height)
{
    pre_block_height_ = pre_block_height;
}

ParseOpreturnData::ParseOpreturnData(const std::string &url, const std::string &auth)
{

   curl_param_->curl_auth = auth;
   curl_param_->need_auth = true;
   curl_param_->curl_url = url;
   filter_type_ = INMEMPOOL;
   action_ = new Action();
}

ParseOpreturnData::~ParseOpreturnData()
{
    delete action_;
}

void ParseOpreturnData::FilterBlock()
{
    switch (filter_type_)
    {
    case ONCHAIN:
        filterBlockOnChain();
        break;
    case INMEMPOOL:
        filterBlockMempool();
        break;
    default:
        break;
    }
}

void ParseOpreturnData::filterBlockOnChain()
{
    json json_post;
    json json_params = json::array();
    //get current the  block height
    StructRpcHeader("getblockcount",json_post);
    json_post["params"] = json_params;
    curl_param_->curl_post_data = json_post.dump();
    if ( !CurlPost(curl_param_) )
    {
        LOG(ERROR) << "RPC getblockcount failed ";
        return;
    }
    json json_response  = json::parse(curl_param_->curl_response);
    json json_result;

    if(json_response["result"].is_null())
    {
        LOG(ERROR) << "getblockcount error " << curl_param_->curl_response ;
        return ;
    }
    uint64_t current_height = json_response["result"].get<uint64_t>();


    //ergodic the block ,and add has op_return tx into memory
    std::string block_hash;
    for( ; pre_block_height_ < current_height; )
    {
        pre_block_height_++;
        StructRpcHeader("getblockhash",json_post);
        json_params.clear();
        json_params.push_back(pre_block_height_);
        json_post["params"] = json_params;
        curl_param_->curl_post_data = json_post.dump();
        if ( !CurlPost(curl_param_) )
        {
            LOG(ERROR) << "RPC getblockhash failed " << pre_block_height_;
            continue;
        }
        json_response = json::parse(curl_param_->curl_response);
        block_hash = json_response["result"].get<std::string>();

        StructRpcHeader("getblock",json_post);
        json_params.clear();
        json_params.push_back(block_hash);
        json_post["params"] = json_params;
        curl_param_->curl_post_data = json_post.dump();
        if ( !CurlPost(curl_param_) )
        {
            LOG(ERROR) << "RPC getblock failed " << pre_block_height_;
            continue;
        }
        json_response = json::parse(curl_param_->curl_response);
        json_result = json_response["result"];
        json json_txs = json_result["tx"];

        std::string tx_hash;
        std::string tx_hex;

        for(unsigned int j = 0; j < json_txs.size(); j++)
        {
            //parse transaction  ,add transaction to memory
            tx_hash = json_txs.at(j).get<std::string>();
            StructRpcHeader("getrawtransaction",json_post);
            json_params.clear();
            json_params.push_back(tx_hash);
            json_post["params"] = json_params;
            curl_param_->curl_post_data = json_post.dump();
            if ( !CurlPost(curl_param_) )
            {
                LOG(ERROR) << "RPC gettransaction failed " << tx_hash;
                continue;
            }
            json_response = json::parse(curl_param_->curl_response);
            if(json_response["result"].is_null())
            {
                LOG(ERROR) << "gettransaction error " << curl_param_->curl_response ;
                continue ;
            }

            tx_hex = json_response["result"].get<std::string>();
            StructRpcHeader("decoderawtransaction",json_post);
            json_params.clear();
            json_params.push_back(tx_hex);
            json_post["params"] = json_params;
            curl_param_->curl_post_data = json_post.dump();
            if ( !CurlPost(curl_param_) )
            {
                LOG(ERROR) << "RPC decoderawtransaction failed " << tx_hex;
                continue;
            }

            json_response = json::parse(curl_param_->curl_response);
            json_result = json_response["result"];
            if(json_result.is_null())
            {
                LOG(ERROR) << "decoderawtransaction error " << curl_param_->curl_response ;
                continue ;
            }
            bool coinbase = (j == 0) ? true : false;
            AddTransaction(json_result, coinbase, pre_block_height_);
        }
    }

}

void ParseOpreturnData::filterBlockMempool()
{
    json json_post;
    json json_params = json::array();
    StructRpcHeader("getrawmempool",json_post);
    json_post["params"] = json_params;

    curl_param_->curl_post_data = json_post.dump();
    if ( !CurlPost(curl_param_) )
    {
        LOG(ERROR) << "RPC getrawmempool failed ";
        return;
    }

    json json_response  = json::parse(curl_param_->curl_response);
    json json_result;
    if(json_response["result"].is_null())
    {
        LOG(ERROR) << "getrawmempool error " << curl_param_->curl_response ;
        return ;
    }

    json json_txs = json_response["result"];
    //std::vector<std::string> ret;
    std::string tx_hash;
    std::string tx_hex;
    for(unsigned int i=0; i<json_txs.size(); ++i)
    {
        tx_hash = json_txs.at(i).get<std::string>();
        std::vector<std::string>::iterator it = std::find(vect_mempool_tx_.begin(), vect_mempool_tx_.end(), tx_hash);
        if (it != vect_mempool_tx_.end())
        {
            continue ;
        }
        StructRpcHeader("getrawtransaction",json_post);
        json_params.clear();
        json_params.push_back(tx_hash);
        json_post["params"] = json_params;
        curl_param_->curl_post_data = json_post.dump();
        if ( !CurlPost(curl_param_) )
        {
            LOG(ERROR) << "gettransaction error " << curl_param_->curl_response ;
            continue ;
        }

        //parse transaction  ,add transaction to memory
        json_response = json::parse(curl_param_->curl_response);
        json_result = json_response["result"];
        if (json_result.is_null())
            continue;
        tx_hex = json_response["result"].get<std::string>();

        StructRpcHeader("decoderawtransaction",json_post);
        json_params.clear();
        json_params.push_back(tx_hex);
        json_post["params"] = json_params;
        curl_param_->curl_post_data = json_post.dump();
        if ( !CurlPost(curl_param_) )
        {
            LOG(ERROR) << "RPC decoderawtransaction failed " << tx_hex;
        }
        json_response = json::parse(curl_param_->curl_response);
        json_result = json_response["result"];
        AddTransaction(json_result);
    }

}

void ParseOpreturnData::AddTransaction(const json &json_tx, bool coinbase, int block_height)
{
    json json_vouts = json_tx["vout"];
    bool has_opreturn = false;
    double value = 0.0;
    for (unsigned int i = 0; i < json_vouts.size(); i++)
    {
        json json_vout  = json_vouts.at(i);
        value = json_vout["value"].get<double>();
        if(IsZero(value))
        {
            has_opreturn = true;
            break;
        }
    }

    std::string txid = json_tx["txid"].get<std::string>();
    Transaction* tx  =new Transaction();
    tx->coinbase = coinbase;
    tx->height = block_height;
    tx->txid = txid;
    json json_vins = json_tx["vin"];
    json json_post;
    json json_params = json::array();

    if (!coinbase)
    {
        for (unsigned int i = 0; i < json_vins.size(); i++)
        {
            json json_vin = json_vins.at(i);
            std::string txid_pre = json_vin["txid"].get<std::string>();
            int pre_vout = json_vin["vout"].get<int>();

            json_params.clear();
            json_params.push_back(txid_pre);
            StructRpcHeader("getrawtransaction",json_post);
            json_post["params"] = json_params;
            curl_param_->curl_post_data = json_post.dump();
            if ( !CurlPost(curl_param_) )
            {
                LOG(ERROR) << "RPC getrawtransaction failed ";
            }
            std::string response = curl_param_->curl_response;
            json json_response = json::parse(response);
            json json_result  = json_response["result"];
            std::string tx_hex = json_response["result"].get<std::string>();

            StructRpcHeader("decoderawtransaction",json_post);
            json_params.clear();
            json_params.push_back(tx_hex);
            json_post["params"] = json_params;
            curl_param_->curl_post_data = json_post.dump();
            if ( !CurlPost(curl_param_) )
            {
                LOG(ERROR) << "RPC decoderawtransaction failed " << tx_hex;
            }
            json_response = json::parse(curl_param_->curl_response);
            json_result = json_response["result"];

            json json_vouts_pre = json_result["vout"];
            TxVin* tx_vin = new TxVin();
            tx_vin->index = i;
            tx_vin->hash = json_result["txid"].get<std::string>();
            json addresses = json_vouts_pre.at(pre_vout)["scriptPubKey"]["addresses"];

            for (unsigned int j = 0; j < addresses.size(); j++)
            {
                ((TxVoutAddress*)tx_vin->pre_vout)->vect_address.push_back(addresses.at(j).get<std::string>());
                tx_vin->pre_vout->index = pre_vout;
            }
            tx->vect_vin.push_back(tx_vin);
        }
    }

    for (unsigned int i = 0; i < json_vouts.size(); i++)
    {
        json json_vout  = json_vouts.at(i);
        value = json_vout["value"].get<double>();
        TxVout* tx_vout = NULL;
        if(IsZero(value))
        {
            tx_vout = new TxVoutOpReturn();
            std::string op_return_data = json_vout["scriptPubKey"]["asm"].get<std::string>();
           ((TxVoutOpReturn*)tx_vout)->op_return_data = op_return_data.substr(10);
           tx_vout->vout_type = OP_RETURN_DATA;

        }
        else
        {
            tx_vout = new TxVoutAddress();
            json addresses = json_vout["scriptPubKey"]["addresses"];
            for (unsigned int j = 0; j < addresses.size(); j++)
            {
                ((TxVoutAddress*)tx_vout)->vect_address.push_back(addresses.at(j).get<std::string>());
            }
            tx_vout->vout_type = ADDRESS;
            ((TxVoutAddress*)tx_vout)->vout_value = value;
        }
        tx_vout->index = json_vout["n"].get<int>();
        tx->vect_vout.push_back(tx_vout);

    }

    if (block_height != -1) {
        vect_utxo_.push_back(tx);
    }

    if (has_opreturn)
    {
        vect_tx_.push_back(tx);

        if (block_height == -1)
            vect_mempool_tx_.push_back(tx->txid);
    }
}

bool ParseOpreturnData::FlushToDB()
{
    if (vect_tx_.size() <= 0 )
    {
        return true;
    }
    Transaction* tx = NULL;
    std::vector<std::string > vect_insert;

    for (unsigned int i =0; i < vect_tx_.size(); i++)
    {
        tx = vect_tx_[i];
        std::string sql_insert  = "INSERT INTO `txinfo` (`height`, `txid`) VALUES ('";
        std::string sql_value = std::to_string(tx->height) + "','" +tx->txid + "');";

        bool update_block_height = false;
        if (tx->height != -1)
        {
            std::string sql_update= "UPDATE `txinfo` SET `height`='" + std::to_string(tx->height) +"' WHERE  `txid`='" + tx->txid +"';";
            std::vector<std::string>::iterator it = std::find(vect_mempool_tx_.begin(), vect_mempool_tx_.end(), tx->txid);
            if (it != vect_mempool_tx_.end())
            {
                update_block_height = true;
                sql_insert = sql_update;
                vect_mempool_tx_.erase(it);
            }
        }

        if (update_block_height)
        {
            vect_insert.push_back(sql_insert);
            continue;
        }


        sql_insert += sql_value;
        vect_insert.push_back(sql_insert);

        TxVin* tx_vin = NULL;
        for (unsigned int j=0; j < tx->vect_vin.size(); j++)
        {
            tx_vin = tx->vect_vin[j];
            sql_insert = "INSERT INTO `txinfo_vin` (`address`, `n`, `txid`) VALUES ('";
            sql_value  = ((TxVoutAddress*)tx_vin->pre_vout)->vect_address.at(0) + "','" + std::to_string(tx_vin->index) + "','" + tx->txid +"');";
            sql_insert += sql_value;
            vect_insert.push_back(sql_insert);
        }

        TxVout* tx_vout = NULL;
        for (unsigned int j=0; j < tx->vect_vout.size(); j++)
        {
            tx_vout = tx->vect_vout.at(j);
            if (tx_vout->vout_type == ADDRESS)
            {
                sql_insert = "INSERT INTO `txinfo_vout` (`content`, `n`, `txid`,`flag`) VALUES ('";
                sql_value  = ((TxVoutAddress*)tx_vout)->vect_address.at(0) + "','" + std::to_string(tx_vout->index) + "','" + tx->txid + "','0');" ;
                sql_insert += sql_value;
                std::cout << sql_insert << std::endl;
                vect_insert.push_back(sql_insert);
                continue;
            }

            sql_insert = "INSERT INTO `txinfo_vout` (`content`, `n`, `txid`,`flag`) VALUES ('";
            sql_value  = ((TxVoutOpReturn*)tx_vout)->op_return_data + "','" + std::to_string(tx_vout->index) + "','" + tx->txid + "','1');" ;
            sql_insert += sql_value;
            vect_insert.push_back(sql_insert);
            ResponseAction(tx,((TxVoutOpReturn*)tx_vout)->op_return_data);


        }
    }
    g_db_mysql->BatchQuerySql(vect_insert);
    vect_tx_.clear();
    return true;
}


bool ParseOpreturnData::FlushUtxoToDB()
{
    if (vect_utxo_.empty())
        return true;
    
    Transaction* tx = NULL;
    std::vector<std::string > vect_sql;

    for (size_t i = 0; i < vect_utxo_.size(); ++i)
    {
        tx = vect_utxo_[i];
        if (tx->height == -1)
        {
            delete tx;
            continue;
        }

        TxVin* tx_vin = NULL;
        if (!tx->coinbase)
        {
            for (size_t j = 0; j < tx->vect_vin.size(); ++j)
            {
                tx_vin = tx->vect_vin[j];

                std::string sql_delete = "DELETE FROM utxo WHERE txid='" + tx_vin->hash 
                                       + "' AND vout=" + std::to_string(tx_vin->pre_vout->index);
                vect_sql.push_back(sql_delete);

            }
        }

        TxVout* tx_vout = NULL;
        for (size_t j = 0; j < tx->vect_vout.size(); ++j)
        {
            tx_vout = tx->vect_vout.at(j);
            if (tx_vout->vout_type == ADDRESS)
            {
                std::string sql_insert = "INSERT INTO utxo (txid,vout,address,value,height,coinbase) VALUES ('" 
                          + tx->txid + "'," 
                          + std::to_string(tx_vout->index) + ",'" 
                          + ((TxVoutAddress*)tx_vout)->vect_address.at(0) + "','"
                          + std::to_string(((TxVoutAddress*)tx_vout)->vout_value) + "'," 
                          + std::to_string(tx->height) + "," 
                          + std::to_string(tx->coinbase ? 1 : 0) + ")";
                vect_sql.push_back(sql_insert);  
            }
        }
        delete tx;

    }
    g_db_mysql->BatchQuerySql(vect_sql);
    vect_utxo_.clear();
    return true;
}


bool ParseOpreturnData::InitMempoolTx(std::vector<std::string> vect_mempool_tx)
{
    vect_mempool_tx_ = vect_mempool_tx;
    return true;
}

bool ParseOpreturnData::ResponseAction(Transaction* tx,const std::string& op_return_data)
{
    NCAct operation = action_->UnSerializeOperation(op_return_data);
    if (!operation)
        return false;
    HandlerScan handler;
    bool ret = false;
    json json_contract;
    NCActA action_a = NULL;
    NCActT action_t = NULL;
    NCActC action_c = NULL;
    std::string txid;
    TxVoutAddress* vout = (TxVoutAddress*)tx->vect_vout[1];
    TxVin* vin  = tx->vect_vin[0];
    TxVoutAddress* pre_vout =(TxVoutAddress*)vin->pre_vout;
    std::string vin_address = pre_vout->vect_address[0];
    bool is_handle = false;
    for (unsigned int i = 0; i < tx->vect_vout.size(); i++ )
    {
        TxVout* vout_tmp = tx->vect_vout.at(i);
        if(vout_tmp->vout_type == ADDRESS )
        {
            vout = (TxVoutAddress*)vout_tmp;
            if (handler.IsMyAddress(vout->vect_address[0])  && !handler.IsMyAddress(vin_address))
            {
                is_handle = true;
                break;
            }
        }
    }

    if ( !is_handle && ActionObject::T2  != operation->operation_code )
    {
        delete operation;
        return false;
    }

    switch (operation->operation_code)
    {
    case ActionObject::C1:
        action_c = (NCActC)operation;
        json_contract["issuer_address"] = vin_address;
        json_contract["contract_address"] = vout->vect_address[0];
        json_contract["contract_name"] = action_c->contract_name;
        json_contract["contract_content"] = action_c->contract_file_hash;
        json_contract["fee"] = 0.00005;
        ret = handler.ContractFormat(json_contract,txid);
        break;
    case ActionObject::A1:
        action_a = (NCActA)operation;
        json_contract["issuer_address"] =  vin_address;
        json_contract["contract_address"] = vout->vect_address[0];
        json_contract["asset_type"] = action_a->asset_type;
        json_contract["asset_id"] = action_a->asset_id;
        json_contract["qty"] = action_a->qty;
        json_contract["fee"] = 0.00012;
        ret = handler.AssetCreation(json_contract,txid);
        break;
    case ActionObject::T2:
        action_t = (NCActT)operation;
        ret = ResponseActionT2(tx,action_t);
        break;
    case ActionObject::T4:
        action_t = (NCActT)operation;
        ret = ResponseActionT4(tx,action_t);
        break;
    default:
        break;
    }

    delete operation;
    return ret;

}

bool ParseOpreturnData::ResponseActionT2(ParseOpreturnData::Transaction *tx, NCActT action_t)
{

    HandlerScan handler;
    std::vector<std::string> vect_address_vin;
    std::string address ;
    TxVoutAddress* vout_address = NULL;
    for(unsigned int i = 0; i < tx->vect_vin.size(); i++)
    {
        vout_address =(TxVoutAddress*)tx->vect_vin[i]->pre_vout;
        address = vout_address->vect_address.at(0);
        std::vector<std::string>::iterator it = std::find(vect_address_vin.begin(), vect_address_vin.end(), address);
        if (it == vect_address_vin.end())
        {
            vect_address_vin.push_back(vout_address->vect_address.at(0));
        }
    }

    std::vector<std::string> vect_address_vout;
    std::string contract_address;
    std::string send_address;
    std::string recieve_address;
    double currency = 0.0;
    uint64_t share_exchange = action_t->party1_token_qty;

    for(unsigned int i = 0; i < tx->vect_vout.size(); i++)
    {
        if(tx->vect_vout[i]->vout_type  == ADDRESS )
        {
            vout_address = (TxVoutAddress*)tx->vect_vout[i];
            vect_address_vout.push_back(vout_address->vect_address.at(0));
            address = vout_address->vect_address.at(0);
            std::vector<std::string>::iterator it = std::find(vect_address_vin.begin(), vect_address_vin.end(), address);
            if (it == vect_address_vin.end())
            {
                contract_address = address;
                currency = vout_address->vout_value;
            }
            else if(address.find(action_t->exchange_fee_address) != std::string::npos)
            {
                send_address = address;
            }
            else
            {
                recieve_address = address;
            }
        }
    }
    if ( !handler.IsMyAddress(contract_address) )
    {
        return false;
    }
    json json_query;
    json_query["address"] = send_address;
    json_query["asset_id"] = action_t->party1_asset_id;
    json_query["contract"] = contract_address;
    uint64_t share_send = handler.QueryContract(json_query);

    if (share_send == 0)
    {
        LOG(ERROR) << "ResponseActionT2 share_send is zero" ;
        return false;
    }

    if (share_send < share_exchange )
    {
        LOG(ERROR) << "ResponseActionT2 share_send "  << share_send << "little share_exchange" << share_exchange ;
        return false;
    }

    share_send -= share_exchange;
    json_query["address"] = recieve_address;
    uint64_t share_recieve = handler.QueryContract(json_query);
    share_recieve += share_exchange;

    json json_contract;
    json_contract["contract_address"] =contract_address;
    json_contract["send_address"] = send_address;
    json_contract["recieve_address"] = recieve_address;
    json_contract["currency"] = currency;
    json_contract["asset_id"] = action_t->party1_asset_id;
    json_contract["qty1"] = share_send;
    json_contract["qty2"] = share_recieve;
    json_contract["fee"] =  0.00005;
    json_contract["txid"] = tx->txid;
    std::string txid;
    bool ret = handler.SettlementToken(json_contract,txid);

    return ret;

}

bool ParseOpreturnData::ResponseActionT4(ParseOpreturnData::Transaction *tx, NCActT action_t)
{
    HandlerScan handler;
    std::string contract_address ;
    TxVoutAddress* vout_address = NULL;
    for(unsigned int i = 0; i < tx->vect_vin.size(); i++)
    {
        vout_address =(TxVoutAddress*)tx->vect_vin[i]->pre_vout;
        contract_address = vout_address->vect_address.at(0);
    }

    std::string send_address;
    std::string recieve_address;

    std::string asset_id = action_t->asset_id;
    uint64_t send_share = action_t->party1_token_qty;
    uint64_t recieve_share =action_t->party2_token_qty;

    double value = 0.0;
    int pos = 0;
    for(unsigned int i = 0; i < tx->vect_vout.size(); i++)
    {
        if(tx->vect_vout[i]->vout_type  == ADDRESS )
        {
            vout_address = (TxVoutAddress*)tx->vect_vout[i];
            std::string address = vout_address->vect_address.at(0);
            if ( address == contract_address )
                continue;
            if ( vout_address->vout_value > value )
            {
                if ( pos == 0)
                {
                    send_address = vout_address->vect_address.at(0);
                    value = vout_address->vout_value;
                }
                else
                {
                    recieve_address = send_address;
                    send_address = vout_address->vect_address.at(0);
                }
            }
            else
            {
                if (recieve_address.empty())
                {
                    recieve_address = vout_address->vect_address.at(0);
                }
                else
                {
                    send_address = recieve_address;
                    recieve_address = vout_address->vect_address.at(0);
                }
            }
        }
    }


    if ( handler.IsMyAddress(send_address) )
    {
        Handler::WalletInfo wallet_info;
        wallet_info.address = send_address;
        wallet_info.asset_id = asset_id;
        wallet_info.contract = contract_address;
        wallet_info.qty = send_share;
        wallet_info.txid = tx->txid;
        wallet_info.type = Action::T4;
        handler.FlushToDB(&wallet_info);
    }

    if( handler.IsMyAddress(recieve_address))
    {
        Handler::WalletInfo wallet_info;
        wallet_info.address = recieve_address;
        wallet_info.asset_id = asset_id;
        wallet_info.contract = contract_address;
        wallet_info.qty = recieve_share;
        wallet_info.txid = tx->txid;
        wallet_info.type = Action::T4;
        handler.FlushToDB(&wallet_info);
    }

    return true;

}






