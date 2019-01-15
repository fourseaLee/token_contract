#include "handlerscan.h"
#include <glog/logging.h>

#define PRE_LENGHT  8 // precise length
#define FIXED_FEE  0.00001 //fixed  charge fee
#define SETTLE_RECIEVE  0.00005 //fixed  recieve get value
static Contract* s_contract  = new Contract();
HandlerScan::HandlerScan()
{

}
bool HandlerScan::ContractFormat(const json &json_contract, std::string &txid)
{
    std::string issuer_address = json_contract["issuer_address"].get<std::string>();
    std::string contract_address = json_contract["contract_address"].get<std::string>();
    std::string contract_name = json_contract["contract_name"].get<std::string>();
    std::string contract_content = json_contract["contract_content"].get<std::string>();
    double fee = json_contract["fee"].get<double>();

    NCAct  action = new Action::ContractOperation();
    NCActC action_c =  (NCActC)action;
    action_c->operation_code = Action::C2;
    action_c->contract_name = contract_name;
    action_c->contract_file_hash = contract_content;

    std::string op_ret_data;
    s_contract->FormatOpReturnData(action,op_ret_data);

    Tx* tx = new Tx();
    tx->map_input_amout_[contract_address] = fee;
    tx->map_output_amount_[issuer_address] = fee;
    tx->PushInputAddress(contract_address);
    tx->PushOutputAddress(issuer_address);
    tx->SetOpReturnData(op_ret_data);
    CreateTransaction(tx,txid);

    delete tx;
    delete action;
    return true;
}

bool HandlerScan::AssetCreation(const json &json_contract, std::string &txid)
{
    std::string issuer_address = json_contract["issuer_address"].get<std::string>();
    std::string contract_address = json_contract["contract_address"].get<std::string>();
    double contract_fee = json_contract["fee"].get<double>();
    std::string asset_id = json_contract["asset_id"].get<std::string>();
    uint64_t qty = json_contract["qty"].get<uint64_t>();

    NCAct action = new Action::AssetOperation();
    NCActA action_a = (NCActA)action;
    action_a->operation_code = Action::A2;
    action_a->asset_type = "SHC";// user_input_contract->asset_type;
    action_a->asset_id = asset_id;
    action_a->asset_revision = 32;
    action_a->voting_system = "M";
    action_a->qty = qty;
    action_a->contract_fee_currency = "BCH";
    action_a->contract_fee_var = 0.01;
    action_a->contract_fee_fixed = 15;

    std::string op_ret_data;
    s_contract->FormatOpReturnData(action,op_ret_data);

    Tx* tx = new Tx();
    tx->map_input_amout_[contract_address] = contract_fee;
    tx->map_output_amount_[issuer_address] = contract_fee;
    tx->PushInputAddress(contract_address);
    tx->PushOutputAddress(issuer_address);
    tx->SetOpReturnData(op_ret_data);
    CreateTransaction(tx,txid);

    WalletInfo* wallet_info = new WalletInfo();
    wallet_info->txid = txid;
    wallet_info->type = ActionObject::A2;
    wallet_info->address = issuer_address;
    wallet_info->asset_id = asset_id;
    wallet_info->contract = contract_address;
    wallet_info->qty = qty;
    FlushToDB(wallet_info);

    delete wallet_info;
    delete tx;
    delete action;
    return true;
}

bool HandlerScan::SettlementToken(const json &json_contract, std::string &txid)
{
    std::string contract_address = json_contract["contract_address"].get<std::string>();
    std::string send_address = json_contract["send_address"].get<std::string>();
    std::string recieve_address = json_contract["recieve_address"].get<std::string>();
    double currency = json_contract["currency"].get<double>();
 //   double contract_fee = json_contract["fee"].get<double>();
    std::string asset_id = json_contract["asset_id"].get<std::string>();
    uint64_t share_send = json_contract["qty1"].get<uint64_t>();
    uint64_t share_recieve = json_contract["qty2"].get<uint64_t>();

    std::string txid_input = json_contract["txid"].get<std::string>();

    NCAct action = new Action::TransferOperation;
    NCActT action_t = (NCActT)action;

    action_t->operation_code = Action::T4;
    action_t->asset_type = "SHC";
    action_t->asset_id = asset_id;
    action_t->party1_token_qty = share_send;
    action_t->party2_token_qty = share_recieve;

    std::string op_ret_data;
    s_contract->FormatOpReturnData(action,op_ret_data);

    LOG(INFO) << "T4 op_return data: " << op_ret_data ;

    Tx* tx = new Tx();
    tx->map_input_amout_[contract_address] = currency;
    tx->map_output_amount_[send_address] = currency - FIXED_FEE - SETTLE_RECIEVE;
    tx->map_output_amount_[recieve_address] = SETTLE_RECIEVE;
    tx->PushInputAddress(contract_address);
    tx->PushOutputAddress(send_address);
    tx->PushOutputAddress(recieve_address);
    tx->SetOpReturnData(op_ret_data);
    CreateTransactionT4(tx,txid_input,txid);

    WalletInfo* wallet_info = new WalletInfo();

    wallet_info->contract = contract_address;
    wallet_info->asset_id = asset_id;
    wallet_info->address = send_address;
    wallet_info->txid = txid;
    wallet_info->type = ActionObject::T4;
    wallet_info->qty = share_send;
    FlushToDB(wallet_info);

    wallet_info->contract = contract_address;
    wallet_info->asset_id = asset_id;
    wallet_info->address = recieve_address;
    wallet_info->txid = txid;
    wallet_info->type = ActionObject::T4;
    wallet_info->qty = share_recieve;
    FlushToDB(wallet_info);

    delete wallet_info;
    delete tx;
    delete action;
    return true;
}

bool HandlerScan::CreateTransactionT4(Tx *tx, const  std::string &txid_input,  std::string &txid_output)
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

    double input_value =0.0;
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

        if (!CurlPost(curl_params_) )
            return false;

        json_response = json::parse(curl_params_->curl_response);
        if (json_response["result"].is_null())
        {
            LOG(INFO)  << curl_params_->curl_post_data;
            LOG(ERROR) << curl_params_->curl_response;
            return false;
        }

        json_result = json_response["result"];
        if (json_result.size() <= 0)
        {
            LOG(INFO)  << "has not UTXO : " <<curl_params_->curl_post_data;
            return false;
        }

        for (unsigned int j = 0; j < json_result.size(); j++ )
        {
            json json_value  = json_result.at(j);
            std::string txid_spent = json_value["txid"].get<std::string>();
            if (txid_spent != txid_input)
                continue;
            int vout = json_value["vout"].get<int>();
            double amount = json_value["amount"].get<double>();
            input_value += amount;
            json json_txid;
            json_txid["txid"] = txid_spent;
            json_txid["vout"] = vout;
            json_txid_array.push_back(json_txid);
        }
    }

    std::string hex_data ;
    tx->GetOpReturnData(hex_data);

    json json_tx_output;
    json_tx_output["data"] = hex_data;
    std::map<std::string,double>::const_iterator cont_it = tx->map_output_amount_.begin();
    for (;cont_it != tx->map_output_amount_.end(); cont_it++ )
    {
        json_tx_output[cont_it->first] = cont_it->second;
    }

    json_params.clear();
    json_params.push_back(json_txid_array);
    json_params.push_back(json_tx_output);

    StructRpcHeader("createrawtransaction",json_post);
    json_post["params"] = json_params;

    curl_params_->curl_post_data  = json_post.dump();
    if (!CurlPost(curl_params_))
        return false;

    json_response = json::parse(curl_params_->curl_response);

    if (json_response["result"].is_null())
    {
        LOG(INFO) << curl_params_->curl_post_data ;
        LOG(ERROR) << curl_params_->curl_response ;
        return false;
    }

    std::string tx_hex = json_response["result"].get<std::string>();
    StructRpcHeader("signrawtransaction",json_post);
    json_params.clear();
    json_params.push_back(tx_hex);
    json_post["params"] = json_params;
    curl_params_->curl_post_data  = json_post.dump();
    if (!CurlPost(curl_params_))
        return false;

    json_response = json::parse(curl_params_->curl_response);
    if(json_response.find("result") == json_response.end())
    {
        LOG(INFO) << curl_params_->curl_post_data ;
        LOG(ERROR) << curl_params_->curl_response ;
        return false;
    }
    if (json_response["result"].is_null())
    {
        LOG(INFO) << curl_params_->curl_post_data ;
        LOG(ERROR) << curl_params_->curl_response ;
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
        return false;
    json_response = json::parse(curl_params_->curl_response);
    if (json_response["result"].is_null())
    {
        LOG(INFO) << curl_params_->curl_post_data ;
        LOG(ERROR) << curl_params_->curl_response ;
        return false;
    }

    txid_output = json_response["result"].get<std::string>();
    return true;
}

