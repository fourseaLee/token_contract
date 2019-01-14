#include "action.h"
#include "contract/contract.h"
#include <bitcoin/bitcoin.hpp>
#include <boost/algorithm/hex.hpp>
#include <glog/logging.h>
#include <iostream>


Action::Action()
{

}

Action::Operation* Action::UnSerializeOperation(const std::string &op_return_hex)
{
    LOG(INFO) << op_return_hex;
    std::string symbol_hex = op_return_hex.substr(0,8);

    std::string symbol;
    boost::algorithm::unhex(symbol_hex, std::back_inserter(symbol));
    if (symbol != "0000")
    {
        return NULL;
    }

    std::string op_code;
    std::string prefix_hex = op_return_hex.substr(8,4);
    boost::algorithm::unhex(prefix_hex, std::back_inserter(op_code));

    Operation* action = NewOperation(op_code,op_return_hex);
    return action;
}

//  static void Contract::BytesToFloat(float& out_float, const std::string& hex_data)
//  {
//      using namespace bc;
//      data_chunk decode_data;
//      decode_base16(decode_data,hex_data);
//      char hex_to_float[4];
//      for(int i = 0; i < 4; ++i)
//      {
//          hex_to_float[i] = decode_data[i];
//      }
//  
//       out_float = *((float*)hex_to_float);
//  }
//  static void Contract::BytesToLong(uint64_t& out_long, const std::string& hex_data)
//  {
//      using namespace bc;
//      data_chunk decode_data;
//      decode_base16(decode_data,hex_data);
//      char hex_to_long[8];
//      for(int i = 0; i < 8; ++i)
//      {
//          hex_to_long[i] = decode_data[i];
//      }
//  
//       out_long = *((uint64_t*)hex_to_long);
//  }
//  static void Contract::BytesToShort(uint16_t& out_short, const std::string& hex_data)
//  {
//      using namespace bc;
//      data_chunk decode_data;
//      decode_base16(decode_data,hex_data);
//      char hex_to_short[2];
//      for(int i = 0; i < 2; ++i)
//      {
//          hex_to_short[i] = decode_data[i];
//      }
//  
//       out_short = *((uint16_t*)hex_to_short);//
//  }
Action::Operation *Action::NewOperation(const std::string& code_serialize,const std::string &op_return_hex)
{
    ContractCode op_code  = ActionObject::CodeUnSerialize(code_serialize);
    Action::Operation* action = NULL;
    bool format = false;

    switch (op_code)
    {
    case C1:
    case C2:
    case C3:
        action = new ContractOperation();
        action->operation_code = op_code;
        format = UnSerializeContract(action,op_code,op_return_hex);
        break;
    case A1:
    case A2:
    case A3:
        action = new AssetOperation();
        action->operation_code = op_code;
        format = UnSerializeAsset(action,op_code,op_return_hex);
        break;
    case T1:
    case T2:
    case T3:
    case T4:
        action = new TransferOperation();
        action->operation_code = op_code;
        format = UnSerializeTransfer(action,op_code,op_return_hex);
        break;
    case G1:
    case G2:
    case G3:
    case G4:
    case G5:
    case G6:
        action = new GovernanceOperation();
        break;
    case E1:
    case E2:
    case E3:
    case E4:
    case E5:
        action = new EstablishOperation();
        break;
    case M1:
    case M2:
        action = new MessageOperation();
        break;
    case R1:
    case R2:
    case R3:
    case R4:
    default:
        break;
    }
    if (!format)
    {
        delete action;
        action = NULL;
        return action;
    }

    action->prefix = code_serialize;
    return action;
}

bool Action::UnSerializeAsset(Action::Operation *action, ActionObject::ContractCode op_code,const std::string &op_return_hex)
{
    LOG(INFO) << "op_return_hex: " << op_return_hex;

    if (op_return_hex.size() < 20)
        return false;

    std::string fields;
    boost::algorithm::unhex(op_return_hex.substr(12), std::back_inserter(fields));
    LOG(INFO) <<"Asset fields : "<< fields;

    std::string filed;
    boost::algorithm::unhex(op_return_hex.substr(12,2), std::back_inserter(filed));
    ((AssetOperation*)action)-> version = filed;
    LOG(INFO)<<"version : "<< ((AssetOperation*)action)-> version;

    filed.clear();
    boost::algorithm::unhex(op_return_hex.substr(14,6), std::back_inserter(filed));
    ((AssetOperation*)action)-> asset_type = filed;
    LOG(INFO)<<"asset_type : "<< ((AssetOperation*)action)-> asset_type;

    // filed.clear();
    // boost::algorithm::unhex(op_return_hex.substr(20,64), std::back_inserter(filed));
    // ((AssetOperation*)action)-> asset_id = filed;
    // LOG(INFO)<<"asset_id : "<< ((AssetOperation*)action)-> asset_id;

    ((TransferOperation*)action)->asset_id = op_return_hex.substr(20,64);
    LOG(INFO)<<"asset_id : "<< ((TransferOperation*)action)-> asset_id;

    if(A1 == op_code)
    {
        if (op_return_hex.size() < 168)
            return false;
        filed.clear();
        boost::algorithm::unhex(op_return_hex.substr(84,4), std::back_inserter(filed));
        ((AssetOperation*)action)-> authorization_flag = filed;
        LOG(INFO)<<"authorization_flag : "<< ((AssetOperation*)action)-> authorization_flag;

        filed.clear();
        boost::algorithm::unhex(op_return_hex.substr(88,2), std::back_inserter(filed));
        ((AssetOperation*)action)-> voting_system = filed;
        LOG(INFO)<<"voting_system : "<< ((AssetOperation*)action)-> voting_system;

        filed.clear();
        boost::algorithm::unhex(op_return_hex.substr(90,2), std::back_inserter(filed));
        ((AssetOperation*)action)-> vote_multiplier = filed;
        LOG(INFO)<<"vote_multiplier : "<< ((AssetOperation*)action)-> vote_multiplier;

        uint64_t qty = 0;
        Contract::BytesToLong(qty,op_return_hex.substr(92,16));
        ((AssetOperation*)action)->qty = qty;
        LOG(INFO)<<"qty: "<< ((AssetOperation*)action)->qty;

        filed.clear();
        boost::algorithm::unhex(op_return_hex.substr(108,6), std::back_inserter(filed));
        ((AssetOperation*)action)-> contract_fee_currency = filed;
        LOG(INFO)<<"contract_fee_currency : "<< ((AssetOperation*)action)-> contract_fee_currency;

        float contract_fee_var = 0;
        Contract::BytesToFloat(contract_fee_var,op_return_hex.substr(114,8));
        ((AssetOperation*)action)->contract_fee_var = contract_fee_var;
        LOG(INFO)<<"contract_fee_var: "<< ((AssetOperation*)action)->contract_fee_var;

        float contract_fee_fixed = 0;
        Contract::BytesToFloat(contract_fee_fixed,op_return_hex.substr(122,8));
        ((AssetOperation*)action)->contract_fee_fixed = contract_fee_fixed;
        LOG(INFO)<<"contract_fee_fixed: "<< ((AssetOperation*)action)->contract_fee_fixed;

        filed.clear();
        boost::algorithm::unhex(op_return_hex.substr(130,38), std::back_inserter(filed));
        ((AssetOperation*)action)-> payload = filed;
        LOG(INFO)<<"payload : "<< ((AssetOperation*)action)-> payload;

    }
    else if(A2 == op_code)
    {
        if (op_return_hex.size() < 188)
            return false;
        uint16_t asset_revision = 0;
        Contract::BytesToShort(asset_revision,op_return_hex.substr(84,4));
        ((AssetOperation*)action)->asset_revision = asset_revision;
        LOG(INFO)<<"asset_revision: "<< ((AssetOperation*)action)->asset_revision;

        filed.clear();
        boost::algorithm::unhex(op_return_hex.substr(88,16), std::back_inserter(filed));
        ((AssetOperation*)action)-> timestamp = filed;
        LOG(INFO)<<"timestamp : "<< ((AssetOperation*)action)-> timestamp;

        filed.clear();
        boost::algorithm::unhex(op_return_hex.substr(104,4), std::back_inserter(filed));
        ((AssetOperation*)action)-> authorization_flag = filed;
        LOG(INFO)<<"authorization_flag : "<< ((AssetOperation*)action)-> authorization_flag;

        filed.clear();
        boost::algorithm::unhex(op_return_hex.substr(108,2), std::back_inserter(filed));
        ((AssetOperation*)action)-> voting_system = filed;
        LOG(INFO)<<"voting_system : "<< ((AssetOperation*)action)-> voting_system;

        filed.clear();
        boost::algorithm::unhex(op_return_hex.substr(110,2), std::back_inserter(filed));
        ((AssetOperation*)action)-> vote_multiplier = filed;
        LOG(INFO)<<"vote_multiplier : "<< ((AssetOperation*)action)-> vote_multiplier;

        uint64_t qty = 0;
        Contract::BytesToLong(qty,op_return_hex.substr(112,16));
        ((AssetOperation*)action)->qty = qty;
        LOG(INFO)<<"qty: "<< ((AssetOperation*)action)->qty;

        filed.clear();
        boost::algorithm::unhex(op_return_hex.substr(128,6), std::back_inserter(filed));
        ((AssetOperation*)action)-> contract_fee_currency = filed;
        LOG(INFO)<<"contract_fee_currency : "<< ((AssetOperation*)action)-> contract_fee_currency;

        float contract_fee_var = 0;
        Contract::BytesToFloat(contract_fee_var,op_return_hex.substr(134,8));
        ((AssetOperation*)action)->contract_fee_var = contract_fee_var;
        LOG(INFO)<<"contract_fee_var: "<< ((AssetOperation*)action)->contract_fee_var;

        float contract_fee_fixed = 0;
        Contract::BytesToFloat(contract_fee_fixed,op_return_hex.substr(142,8));
        ((AssetOperation*)action)->contract_fee_fixed = contract_fee_fixed;
        LOG(INFO)<<"contract_fee_fixed: "<< ((AssetOperation*)action)->contract_fee_fixed;

        filed.clear();
        boost::algorithm::unhex(op_return_hex.substr(150,38), std::back_inserter(filed));
        ((AssetOperation*)action)-> payload = filed;
        LOG(INFO)<<"payload : "<< ((AssetOperation*)action)-> payload;
    }
    else 
    {
        LOG(INFO)<<"Asset no op_code";
        return false;
    }
    return true;
}

 bool Action::UnSerializeContract(Action::Operation *action, ActionObject::ContractCode op_code,const std::string &op_return_hex) 
 {
 
     if (op_return_hex.size() < 162)
     {
         return false;
     }
     LOG(INFO) << "op_return_hex: " << op_return_hex;
 
     std::string fileds;
     boost::algorithm::unhex(op_return_hex.substr(12), std::back_inserter(fileds));
     LOG(INFO)<<"Contract fileds :" << fileds <<std::endl;
 
     std::string filed;
     boost::algorithm::unhex(op_return_hex.substr(12,2), std::back_inserter(filed));
     ((ContractOperation*)action)-> version = filed;
     LOG(INFO)<<"version : "<< ((ContractOperation*)action)-> version;
 
     ((ContractOperation*)action)-> contract_name = op_return_hex.substr(14,64);
     LOG(INFO)<<"contract_name : "<< ((ContractOperation*)action)->contract_name;
 
     ((ContractOperation*)action)-> contract_file_hash = op_return_hex.substr(78,64);
     LOG(INFO)<<"contract_file_hash : "<< ((ContractOperation*)action)->contract_file_hash;
 
     filed.clear();
     boost::algorithm::unhex(op_return_hex.substr(142,10), std::back_inserter(filed));
     ((ContractOperation*)action)-> governing_law = filed;
     LOG(INFO)<<"governing_law : "<< ((ContractOperation*)action)->governing_law;
 
     filed.clear();
     boost::algorithm::unhex(op_return_hex.substr(152,10), std::back_inserter(filed));
     ((ContractOperation*)action)-> jurisdiction = filed;
     LOG(INFO)<<"jurisdiction : "<< ((ContractOperation*)action)->jurisdiction;
 
     
     if(C1 == op_code)
     {
         if (op_return_hex.size() < 388)
             return false;
         filed.clear();
         boost::algorithm::unhex(op_return_hex.substr(162,16), std::back_inserter(filed));
         ((ContractOperation*)action)-> contract_expriration = filed;
         LOG(INFO)<<"contract_expriration : "<< ((ContractOperation*)action)->contract_expriration;
 
         filed.clear();
         boost::algorithm::unhex(op_return_hex.substr(178,140), std::back_inserter(filed));
         ((ContractOperation*)action)-> url = filed;
         LOG(INFO)<<"url : "<< ((ContractOperation*)action)->url;
 
         filed.clear();
         boost::algorithm::unhex(op_return_hex.substr(318,32), std::back_inserter(filed));
         ((ContractOperation*)action)-> issuer_id = filed;
         LOG(INFO)<<"issuer_id : "<< ((ContractOperation*)action)->issuer_id;
 
         filed.clear();
         boost::algorithm::unhex(op_return_hex.substr(350,2), std::back_inserter(filed));
         ((ContractOperation*)action)-> issuer_type = filed;
         LOG(INFO)<<"issuer_type : "<< ((ContractOperation*)action)->issuer_type;
 
         filed.clear();
         boost::algorithm::unhex(op_return_hex.substr(352,4), std::back_inserter(filed));
         ((ContractOperation*)action)->authorization_flag = filed;
         LOG(INFO)<<"authorization_flag: "<< ((ContractOperation*)action)->authorization_flag;
 
         filed.clear();
         boost::algorithm::unhex(op_return_hex.substr(356,2), std::back_inserter(filed));
         ((ContractOperation*)action)->voting_system = filed;
         LOG(INFO)<<"voting_system:"<< ((ContractOperation*)action)->voting_system;
 
         float initialtive_threshold = 0;
         Contract::BytesToFloat(initialtive_threshold,op_return_hex.substr(358,8));
         ((ContractOperation*)action)->initialtive_threshold = initialtive_threshold;
         LOG(INFO)<<"initialtive_threshold: "<< ((ContractOperation*)action)->initialtive_threshold;
 
         filed.clear();
         boost::algorithm::unhex(op_return_hex.substr(366,6), std::back_inserter(filed));
         ((ContractOperation*)action)->initialtive_threshold_currency = filed;
         LOG(INFO)<<"initialtive_threshold_currency:"<< ((ContractOperation*)action)->initialtive_threshold_currency;
 
         uint64_t restricted_qty = 0;
         Contract::BytesToLong(restricted_qty,op_return_hex.substr(372,16));
         ((ContractOperation*)action)->restricted_qty = restricted_qty;
         LOG(INFO)<<"restricted_qty: "<< ((ContractOperation*)action)->restricted_qty;
     }
     else if(C2 == op_code)
     {
 
         if (op_return_hex.size() < 440)
             return false;
 
         filed.clear();
         boost::algorithm::unhex(op_return_hex.substr(162,16), std::back_inserter(filed));
         ((ContractOperation*)action)-> timestamp = filed;
         LOG(INFO)<<"timestamp : "<< ((ContractOperation*)action)->timestamp;
 
         filed.clear();
         boost::algorithm::unhex(op_return_hex.substr(178,16), std::back_inserter(filed));
         ((ContractOperation*)action)-> contract_expriration = filed;
         LOG(INFO)<<"contract_expriration : "<< ((ContractOperation*)action)->contract_expriration;
         filed.clear();
         boost::algorithm::unhex(op_return_hex.substr(194,140), std::back_inserter(filed));
         ((ContractOperation*)action)-> url = filed;
         LOG(INFO)<<"url : "<< ((ContractOperation*)action)->url;
 
 
         filed.clear();
         boost::algorithm::unhex(op_return_hex.substr(334,4), std::back_inserter(filed));
         ((ContractOperation*)action)-> contrac_revision = filed;
         LOG(INFO)<<"contrac_revision : "<< ((ContractOperation*)action)->contrac_revision;
 
         filed.clear();
         boost::algorithm::unhex(op_return_hex.substr(338,32), std::back_inserter(filed));
         ((ContractOperation*)action)-> issuer_id = filed;
         LOG(INFO)<<"issuer_id : "<< ((ContractOperation*)action)-> issuer_id;
 
         filed.clear();
         boost::algorithm::unhex(op_return_hex.substr(370,2), std::back_inserter(filed));
         ((ContractOperation*)action)-> issuer_type = filed;
         LOG(INFO)<<"issuer_type : "<< ((ContractOperation*)action)->issuer_type;
 
         filed.clear();
         boost::algorithm::unhex(op_return_hex.substr(372,32), std::back_inserter(filed));
         ((ContractOperation*)action)->contract_operator_id = filed;
         LOG(INFO)<<"contract_operator_id : "<< ((ContractOperation*)action)->contract_operator_id;
         filed.clear();
         boost::algorithm::unhex(op_return_hex.substr(404,4), std::back_inserter(filed));
         ((ContractOperation*)action)->authorization_flag = filed;
         LOG(INFO)<<"authorization_flag: "<< ((ContractOperation*)action)->authorization_flag;
 
         filed.clear();
         boost::algorithm::unhex(op_return_hex.substr(408,2), std::back_inserter(filed));
         ((ContractOperation*)action)->voting_system = filed;
         LOG(INFO)<<"voting_system:"<< ((ContractOperation*)action)->voting_system;
 
         float initialtive_threshold = 0; 
         Contract::BytesToFloat(initialtive_threshold,op_return_hex.substr(410,8));
         ((ContractOperation*)action)->initialtive_threshold = initialtive_threshold;
         LOG(INFO)<<"initialtive_threshold: "<< ((ContractOperation*)action)->initialtive_threshold;
 
         filed.clear();
         boost::algorithm::unhex(op_return_hex.substr(418,6), std::back_inserter(filed));
         ((ContractOperation*)action)->initialtive_threshold_currency = filed;
         LOG(INFO)<<"initialtive_threshold_currency:"<< ((ContractOperation*)action)->initialtive_threshold_currency;
 
         uint64_t restricted_qty = 0;
         Contract::BytesToLong(restricted_qty,op_return_hex.substr(424,16));
         ((ContractOperation*)action)->restricted_qty = restricted_qty;
         LOG(INFO)<<"restricted_qty: "<< ((ContractOperation*)action)->restricted_qty;
     }
     else
     {
         LOG(INFO)<<"Contract no op_code";
         return false;
     }
     return true;
 }
 
 bool Action::UnSerializeTransfer(Action::Operation *action, ActionObject::ContractCode op_code,const std::string &op_return_hex)
 {
     LOG(INFO) << "op_return_hex: " << op_return_hex;
 
     if (op_return_hex.size() < 14)
         return false;
 
     std::string fileds;
     boost::algorithm::unhex(op_return_hex.substr(12), std::back_inserter(fileds));
     LOG(INFO)<<"Transfer fileds :" << fileds <<std::endl;
 
     std::string filed;
     boost::algorithm::unhex(op_return_hex.substr(12,2), std::back_inserter(filed));
     ((TransferOperation*)action)-> version = filed;
     LOG(INFO)<<"version : "<< ((TransferOperation*)action)-> version;
 
 
     if(T2 == op_code)
     {
 
         if (op_return_hex.size() < 222)
             return false;
 
         filed.clear();
         boost::algorithm::unhex(op_return_hex.substr(14,6), std::back_inserter(filed));
         ((TransferOperation*)action)->party1_asset_type = filed;
         LOG(INFO)<<"asset_type : "<< ((TransferOperation*)action)-> party1_asset_type;
 
         // filed.clear();
         // boost::algorithm::unhex(op_return_hex.substr(20,64), std::back_inserter(filed));
         // ((TransferOperation*)action)-> party1_asset_id = filed;
         // LOG(INFO)<<"asset_id : "<< ((TransferOperation*)action)-> party1_asset_id;
 
         ((TransferOperation*)action)->party1_asset_id = op_return_hex.substr(20,64);
         LOG(INFO)<<"asset_id : "<< ((TransferOperation*)action)-> party1_asset_id;
 
         uint64_t party1_token_qty = 0;
         Contract::BytesToLong(party1_token_qty,op_return_hex.substr(84,16));
         ((TransferOperation*)action)->party1_token_qty = party1_token_qty;
         LOG(INFO)<<"party1_token_qty: "<< ((TransferOperation*)action)->party1_token_qty;
 
         filed.clear();
         boost::algorithm::unhex(op_return_hex.substr(100,16), std::back_inserter(filed));
         ((TransferOperation*)action)-> offer_valid_until = filed;
         LOG(INFO)<<"offer_valid_until : "<< ((TransferOperation*)action)-> offer_valid_until;
 
         filed.clear();
         boost::algorithm::unhex(op_return_hex.substr(116,6), std::back_inserter(filed));
         ((TransferOperation*)action)->exchange_fee_currency = filed;
         LOG(INFO)<<"exchange_fee_currency : "<< ((TransferOperation*)action)-> exchange_fee_currency;
 
         float exchange_fee_var = 0;
         Contract::BytesToFloat(exchange_fee_var,op_return_hex.substr(122,16));
         ((TransferOperation*)action)->exchange_fee_var = exchange_fee_var;
         LOG(INFO)<<"exchange_fee_var: "<< ((TransferOperation*)action)->exchange_fee_var;
 
         float exchange_fee_fixed = 0;
         Contract::BytesToFloat(exchange_fee_fixed,op_return_hex.substr(138,16));
         ((TransferOperation*)action)->exchange_fee_fixed = exchange_fee_fixed;
         LOG(INFO)<<"exchange_fee_fixed: "<< ((TransferOperation*)action)->exchange_fee_fixed;
 
         filed.clear();
         boost::algorithm::unhex(op_return_hex.substr(138,84), std::back_inserter(filed));
         ((TransferOperation*)action)->exchange_fee_address = filed;
         LOG(INFO)<<"exchange_fee_address : "<< ((TransferOperation*)action)-> exchange_fee_address;
     }
     else if(T4 == op_code)
     {
         if (op_return_hex.size() < 122)
             return false;
         filed.clear();
         boost::algorithm::unhex(op_return_hex.substr(14,6), std::back_inserter(filed));
         ((TransferOperation*)action)-> asset_type = filed;
         LOG(INFO)<<"asset_type : "<< ((TransferOperation*)action)-> asset_type;
 
         // filed.clear();
         // boost::algorithm::unhex(op_return_hex.substr(20,64), std::back_inserter(filed));
         // ((TransferOperation*)action)-> asset_id = filed;
         // LOG(INFO)<<"asset_id : "<< ((TransferOperation*)action)-> asset_id;
 
         ((TransferOperation*)action)->asset_id = op_return_hex.substr(20,64);
         LOG(INFO)<<"asset_id : "<< ((TransferOperation*)action)-> asset_id;
 
         uint64_t party1_token_qty = 0;
         Contract::BytesToLong(party1_token_qty,op_return_hex.substr(84,16));
         ((TransferOperation*)action)->party1_token_qty = party1_token_qty;
         LOG(INFO)<<"party1_token_qty: "<< ((TransferOperation*)action)->party1_token_qty;
 
         uint64_t party2_token_qty = 0;
         Contract::BytesToLong(party2_token_qty,op_return_hex.substr(100,16));
         ((TransferOperation*)action)->party2_token_qty = party2_token_qty;
         LOG(INFO)<<"party2_token_qty: "<< ((TransferOperation*)action)->party2_token_qty;
 
         filed.clear();
         boost::algorithm::unhex(op_return_hex.substr(116,16), std::back_inserter(filed));
         ((TransferOperation*)action)-> timestamp = filed;
         LOG(INFO)<<"timestamp : "<< ((TransferOperation*)action)-> timestamp;
     }
     else
     {
         LOG(INFO)<<"Transfer no op_code";
         return false;
     }
     return true;
 }
