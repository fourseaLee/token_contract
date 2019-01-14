
#include "contract.h"
#include <glog/logging.h>
#include <boost/algorithm/hex.hpp>


Contract::Contract()
{
    contract_type_ = UNDEFINE_CONTRACT;
}

Contract::~Contract()
{

    for (unsigned int i = 0; i < vect_contract_actions_.size(); i++)
    {
        delete vect_contract_actions_[i];
    }

}

void Contract::SetContractAddress(const std::string &contract_address)
{
    contract_address_ = contract_address;
}

void Contract::GetContractAddress(std::string &contract_address)
{
    contract_address = contract_address_;
}

void Contract::SetTokenSendAddress(const std::string &token_send_address)
{
    token_send_address_ = token_send_address;
}

void Contract::GetTokenSendAddress( std::string &token_send_address)
{
    token_send_address = token_send_address_;
}


void Contract::SetTokenRecieveAddress(const std::string &token_recieve_address)
{
    token_recieve_address_ = token_recieve_address;
}

void Contract::GetTokenRecieveAddress( std::string &token_recieve_address)
{
    token_recieve_address = token_recieve_address_;
}

void Contract::SetBchCountT2(double bch_count_T2)
{
    bch_count_T2_ = bch_count_T2;
}

void Contract::GetBchCountT2(double &bch_count_T2)
{
    bch_count_T2 = bch_count_T2_;
}


void Contract::PushAction(NCAct contact_action)
{
    vect_contract_actions_.push_back(contact_action);
}

std::vector<Tx *> Contract::ActionsToBlockChainTxs()
{
    std::vector<Tx* > vect_txs_ret;
    switch (contract_type_)
    {
    case Contract::SHC_CREATE:
        SHCCreate(vect_txs_ret);
        break;
    case Contract::SHC_TRANSFER:
        SHCTransfer(vect_txs_ret);
        break;
    default:
        break;
    }
    return vect_txs_ret;
}

void Contract::SetContractType(Contract::ContractType contract_type)
{
    contract_type_ = contract_type;
}

int Contract::GetContractType()
{
    return contract_type_;
}

void Contract::SetIssuerAddress(const std::string &issuer_address)
{
    issuer_address_ = issuer_address;
}

void Contract::GetIssuerAddress( std::string &issuer_address)
{
    issuer_address = issuer_address_;
}

void Contract::FloatToBytes(const float float_value, std::string &hex_data)
{
    unsigned char byte_array[4];
    char* pchar=(char*)&float_value;
    for(unsigned int i=0;i<sizeof(float);i++)
    {
        byte_array[i] = *pchar++;
    }
    using namespace bc;
    data_chunk data_bignum;
    for (int i=0; i < 4; i++)
    {
        data_bignum.push_back(byte_array[i]);
    }
    hex_data = encode_base16(data_bignum);
}


void Contract::LongToBytes(const uint64_t long_value, std::string& hex_data)
{
    unsigned char byte_array[8];
    char* pchar=(char*)&long_value;
    for(unsigned int i=0;i<sizeof(uint64_t);i++)
    {
        byte_array[i] = *pchar++;
    }
    using namespace bc;
    data_chunk data_bignum;
    for (int i=0; i < 8; i++)
    {
        data_bignum.push_back(byte_array[i]);
    }
    hex_data = encode_base16(data_bignum);

}


void Contract::ShortToBytes(const uint16_t short_value,std::string& hex_data)
{
    unsigned char byte_array[2];
    char* pchar=(char*)&short_value;
    for(unsigned int i=0;i<sizeof(uint16_t);i++)
    {
        byte_array[i] = *pchar++;
    }

    using namespace bc;
    data_chunk data_bignum;
    for (int i=0; i < 2; i++)
    {
        data_bignum.push_back(byte_array[i]);
    }
    hex_data = encode_base16(data_bignum);
}

void Contract::BytesToFloat(float& float_value, const std::string& hex_data)
{
    using namespace bc;
    data_chunk decode_data;
    decode_base16(decode_data,hex_data);
    char hex_to_float[4];
    for(int i = 0; i < 4; ++i)
    {
        hex_to_float[i] = decode_data[i];
    }

     float_value = *((float*)hex_to_float);
}
void Contract::BytesToLong(uint64_t& long_value, const std::string& hex_data)
{
    using namespace bc;
    data_chunk decode_data;
    decode_base16(decode_data,hex_data);
    char hex_to_long[8];
    for(int i = 0; i < 8; ++i)
    {
        hex_to_long[i] = decode_data[i];
    }

     long_value = *((uint64_t*)hex_to_long);
}
void Contract::BytesToShort(uint16_t& short_value, const std::string& hex_data)
{
    using namespace bc;
    data_chunk decode_data;
    decode_base16(decode_data,hex_data);
    char hex_to_short[2];
    for(int i = 0; i < 2; ++i)
    {
        hex_to_short[i] = decode_data[i];
    }

     short_value = *((uint16_t*)hex_to_short);//
}

bool Contract::SHCCreate(std::vector<Tx *> &vect_tx)
{
    if(vect_contract_actions_.size() != 4)
    {
        LOG(ERROR) << "SHC transaction must be 4 ,but this is " << vect_tx.size() ;
        return false;
    }

    NCAct  act = vect_contract_actions_.at(0);
    if (act->operation_code != ActionObject::C1)
    {
        LOG(ERROR) << "SHC error operation code, must be  C1, but is " << act->operation_code;
        return false;
    }
    std::string op_return_data;
    FormatOpReturnData(act,op_return_data);
    Tx* tx = new Tx();
    tx->PushInputAddress(issuer_address_);
    tx->PushOutputAddress(contract_address_);
    tx->map_input_amout_[issuer_address_] = 0.0005;
    tx->map_output_amount_[contract_address_] = 0.0005;
    tx->SetOpReturnData(op_return_data);
    vect_tx.push_back(tx);

    act =  vect_contract_actions_.at(1);
    if (act->operation_code != ActionObject::C2)
    {
        LOG(ERROR) << "SHC error operation code, must be  C2, but is " << act->operation_code;
        return false;
    }
    FormatOpReturnData(act,op_return_data);
    tx =  NULL;
    tx = new Tx();
    tx->PushInputAddress(contract_address_);
    tx->PushOutputAddress(issuer_address_);
    tx->map_input_amout_[contract_address_] = 0.00005;
    tx->map_output_amount_[issuer_address_] = 0.00005;
    tx->SetOpReturnData(op_return_data);
    vect_tx.push_back(tx);

    act =  vect_contract_actions_.at(2);
    if (act->operation_code != ActionObject::A1)
    {
        LOG(ERROR) << "SHC error operation code, must be  C2, but is " << act->operation_code;
        return false;
    }
    FormatOpReturnData(act,op_return_data);
    tx =  NULL;
    tx = new Tx();
    tx->PushInputAddress(issuer_address_);
    tx->PushOutputAddress(contract_address_);
    tx->map_input_amout_[issuer_address_] = 0.00012;
    tx->map_output_amount_[contract_address_] = 0.00012;
    tx->SetOpReturnData(op_return_data);
    vect_tx.push_back(tx);

    act =  vect_contract_actions_.at(3);
    if (act->operation_code != ActionObject::A2)
    {
        LOG(ERROR) << "SHC error operation code, must be  C2, but is " << act->operation_code;
        return false;
    }
    FormatOpReturnData(act,op_return_data);
    tx =  NULL;
    tx = new Tx();
    tx->PushInputAddress(contract_address_);
    tx->PushOutputAddress(issuer_address_);
    tx->map_input_amout_[contract_address_] = 0.00005;
    tx->map_output_amount_[issuer_address_] = 0.00005;
    tx->SetOpReturnData(op_return_data);
    vect_tx.push_back(tx);

    return true;
}

bool Contract::SHCTransfer(std::vector<Tx *> &vect_tx)
{

    if(vect_contract_actions_.size() != 2)
    {
        LOG(ERROR) << "SHC transaction must be 2 ,but this is " << vect_tx.size() ;
        return false;
    }

    NCAct  act = vect_contract_actions_.at(0);
    if (act->operation_code != ActionObject::T2)
    {
        LOG(ERROR) << "SHC error operation code, must be T2, but is " << act->operation_code;
        return false;
    }
    std::string op_return_data;
    FormatOpReturnData(act,op_return_data);
    Tx* tx = new Tx();
    tx->PushInputAddress(token_send_address_);
    tx->PushInputAddress(token_recieve_address_);
    tx->PushOutputAddress(contract_address_);

    tx->PushOutputAddress(token_recieve_address_);
    tx->map_input_amout_[token_send_address_] = 0.00005;
    tx->map_input_amout_[token_recieve_address_] = bch_count_T2_ +  0.00012;
    tx->map_output_amount_[contract_address_] = bch_count_T2_ + 0.00012;
    tx->map_output_amount_[token_recieve_address_] = 0.00005;
    tx->SetOpReturnData(op_return_data);
    vect_tx.push_back(tx);

    act =  vect_contract_actions_.at(1);
    if (act->operation_code != ActionObject::T4)
    {
        LOG(ERROR) << "SHC error operation code, must be  T4, but is " << act->operation_code;
        return false;
    }
    FormatOpReturnData(act,op_return_data);
    tx =  NULL;
    tx = new Tx();
    tx->PushInputAddress(contract_address_);
    tx->PushOutputAddress(token_send_address_);
    tx->PushOutputAddress(token_recieve_address_);
    tx->map_input_amout_[contract_address_] = bch_count_T2_ + 0.00005;
    tx->map_output_amount_[contract_address_] = 0.00005;
    tx->map_output_amount_[token_send_address_] = bch_count_T2_;
    tx->SetOpReturnData(op_return_data);
    vect_tx.push_back(tx);
    return true;
}

bool Contract::FormatOpReturnData(NCAct action, std::string &op_return_data)
{
    using namespace boost::algorithm;
    op_return_data.clear();
    std::string op_code = ActionObject::CodeSerialize(action->operation_code);
    std::string hex_code;
    hex(op_code, std::back_inserter(hex_code));
    std::string hex_symbol;
    hex(action->symbol, std::back_inserter(hex_symbol));
    op_return_data += hex_symbol;
    op_return_data += hex_code;
    std::string hex_version ;
    hex(action->version, std::back_inserter(hex_version));
    op_return_data += hex_version;

    if (action->operation_code == ActionObject::C1)
    {
        NCActC action_c =(NCActC)action;
        std::string hex_data;
        // std::cout<<"contract_name:"<<action_c->contract_name<<std::endl;
        ContractObject::GetContractHash(action_c->contract_name,hex_data);
        // hex(action_c->contract_name, std::back_inserter(hex_data));
        op_return_data += hex_data;
        hex_data.clear();
        ContractObject::GetContractHash(action_c->contract_file_hash,hex_data);
        // hex(action_c->contract_file_hash, std::back_inserter(hex_data));
        op_return_data += hex_data;
        hex_data.clear();
        hex(action_c->governing_law, std::back_inserter(hex_data));
        op_return_data += hex_data;

        hex_data.clear();
        hex(action_c->jurisdiction, std::back_inserter(hex_data));
        op_return_data += hex_data;

        hex_data.clear();
        hex(action_c->contract_expriration, std::back_inserter(hex_data));
        op_return_data += hex_data;
        hex_data.clear();
        hex(action_c->url, std::back_inserter(hex_data));
        op_return_data += hex_data;
        hex_data.clear();
        hex(action_c->issuer_id, std::back_inserter(hex_data));
        op_return_data += hex_data;
        hex_data.clear();
        hex(action_c->issuer_type, std::back_inserter(hex_data));
        op_return_data += hex_data;
        // hex_data.clear();
        // hex(action_c->contract_operator_id, std::back_inserter(hex_data));
        // op_return_data += hex_data;
        hex_data.clear();
        hex(action_c->authorization_flag, std::back_inserter(hex_data));
        op_return_data += hex_data;
        hex_data.clear();
        hex(action_c->voting_system, std::back_inserter(hex_data));
        op_return_data += hex_data;
        hex_data.clear();
        FloatToBytes(action_c->initialtive_threshold,hex_data);
        std::cout << "initialtive_threshold :" << hex_data << std::endl;
        op_return_data += hex_data;
        hex_data.clear();
        hex(action_c->initialtive_threshold_currency, std::back_inserter(hex_data));
        op_return_data += hex_data;
        hex_data.clear();
        LongToBytes(action_c->restricted_qty,hex_data);
        op_return_data += hex_data;
        hex_data.clear();

    }
    else if (action->operation_code == ActionObject::C2)
    {
        NCActC action_c =(NCActC)action;
        std::string hex_data;

        // static bool GetContractHash(const std::string& contract_content, std::string &contract_hash);
        ContractObject::GetContractHash(action_c->contract_name,hex_data);
        // hex(action_c->contract_name, std::back_inserter(hex_data));
        op_return_data += hex_data;
        hex_data.clear();
        ContractObject::GetContractHash(action_c->contract_file_hash,hex_data);
        // hex(action_c->contract_file_hash, std::back_inserter(hex_data));
        op_return_data += hex_data;
        hex_data.clear();
        hex(action_c->governing_law, std::back_inserter(hex_data));
        op_return_data += hex_data;
        hex_data.clear();
        hex(action_c->jurisdiction, std::back_inserter(hex_data));
        op_return_data += hex_data;
        hex_data.clear();
        hex(action_c->timestamp, std::back_inserter(hex_data));
        op_return_data += hex_data;
        hex_data.clear();
        hex(action_c->contract_expriration, std::back_inserter(hex_data));
        op_return_data += hex_data;
        hex_data.clear();
        hex(action_c->url, std::back_inserter(hex_data));
        op_return_data += hex_data;
        hex_data.clear();
        hex(action_c->contrac_revision, std::back_inserter(hex_data));
        op_return_data += hex_data;
        hex_data.clear();
        hex(action_c->issuer_id, std::back_inserter(hex_data));
        op_return_data += hex_data;
        hex_data.clear();
        hex(action_c->issuer_type, std::back_inserter(hex_data));
        op_return_data += hex_data;
        hex_data.clear();
        hex(action_c->contract_operator_id, std::back_inserter(hex_data));
        op_return_data += hex_data;
        hex_data.clear();
        hex(action_c->authorization_flag, std::back_inserter(hex_data));
        op_return_data += hex_data;
        hex_data.clear();
        hex(action_c->voting_system, std::back_inserter(hex_data));
        op_return_data += hex_data;
        hex_data.clear();
        FloatToBytes(action_c->initialtive_threshold,hex_data);
        op_return_data += hex_data;
        hex_data.clear();
        hex(action_c->initialtive_threshold_currency, std::back_inserter(hex_data));
        op_return_data += hex_data;
        hex_data.clear();
        LongToBytes(action_c->restricted_qty,hex_data);
        op_return_data += hex_data;
        hex_data.clear();
    }
    else if (action->operation_code == ActionObject::A1)
    {

        NCActA action_a =(NCActA)action;
        std::string hex_data;
        hex(action_a->asset_type, std::back_inserter(hex_data));
        op_return_data += hex_data;
        // hex_data.clear();
        // hex(action_a->asset_id, std::back_inserter(hex_data));
        // ContractObject::GetContractHash(action_a->asset_id,hex_data);
        op_return_data += action_a->asset_id;
        hex_data.clear();
        hex(action_a->authorization_flag, std::back_inserter(hex_data));
        op_return_data += hex_data;
        hex_data.clear();
        hex(action_a->voting_system, std::back_inserter(hex_data));
        op_return_data += hex_data;
        hex_data.clear();
        hex(action_a->vote_multiplier, std::back_inserter(hex_data));
        op_return_data += hex_data;
        hex_data.clear();
        LongToBytes(action_a->qty,hex_data);
        op_return_data += hex_data;
        hex_data.clear();
        hex(action_a->contract_fee_currency, std::back_inserter(hex_data));
        op_return_data += hex_data;
        hex_data.clear();
        FloatToBytes(action_a->contract_fee_var,hex_data);
        op_return_data += hex_data;
        hex_data.clear();
        FloatToBytes(action_a->contract_fee_fixed,hex_data);
        op_return_data += hex_data;
        hex_data.clear();
        hex(action_a->payload, std::back_inserter(hex_data));
        op_return_data += hex_data;
        hex_data.clear();
    }
    else if (action->operation_code == ActionObject::A2)
    {
        NCActA action_a =(NCActA)action;
        std::string hex_data;
        hex(action_a->asset_type, std::back_inserter(hex_data));
        op_return_data += hex_data;
        // hex_data.clear();
        // hex(action_a->asset_id, std::back_inserter(hex_data));
        // ContractObject::GetContractHash(action_a->asset_id,hex_data);
        op_return_data += action_a->asset_id;
        hex_data.clear();
        ShortToBytes(action_a->asset_revision,hex_data);
        op_return_data += hex_data;
        hex_data.clear();
        hex(action_a->timestamp, std::back_inserter(hex_data));
        op_return_data += hex_data;
        hex_data.clear();
        hex(action_a->authorization_flag, std::back_inserter(hex_data));
        op_return_data += hex_data;
        hex_data.clear();
        hex(action_a->voting_system, std::back_inserter(hex_data));
        op_return_data += hex_data;
        hex_data.clear();
        hex(action_a->vote_multiplier, std::back_inserter(hex_data));
        op_return_data += hex_data;
        hex_data.clear();
        LongToBytes(action_a->qty,hex_data);
        op_return_data += hex_data;
        hex_data.clear();
        hex(action_a->contract_fee_currency, std::back_inserter(hex_data));
        op_return_data += hex_data;
        hex_data.clear();
        FloatToBytes(action_a->contract_fee_var,hex_data);
        op_return_data += hex_data;
        hex_data.clear();
        FloatToBytes(action_a->contract_fee_fixed,hex_data);
        op_return_data += hex_data;
        hex_data.clear();
        hex(action_a->payload, std::back_inserter(hex_data));
        op_return_data += hex_data;
        hex_data.clear();
    }
    else if (action->operation_code == ActionObject::T2)
    {
        NCActT action_t =(NCActT)action;
        std::string hex_data;
        hex(action_t->party1_asset_type, std::back_inserter(hex_data));
        op_return_data += hex_data;
        // hex_data.clear();
        // hex(action_t->party1_asset_id, std::back_inserter(hex_data));
        // ContractObject::GetContractHash(action_t->party1_asset_id,hex_data);
        op_return_data += action_t->party1_asset_id;
        // op_return_data += hex_data;
        hex_data.clear();
        LongToBytes(action_t->party1_token_qty,hex_data);
        op_return_data += hex_data;
        hex_data.clear();
        hex(action_t->offer_valid_until, std::back_inserter(hex_data));
        op_return_data += hex_data;
        hex_data.clear();
        hex(action_t->exchange_fee_currency, std::back_inserter(hex_data));
        op_return_data += hex_data;
        hex_data.clear();
        FloatToBytes(action_t->exchange_fee_var,hex_data);
        op_return_data += hex_data;
        // hex_data.clear();
        // FloatToBytes(action_t->exchange_fee_fixed,hex_data);
        op_return_data += hex_data;
        hex_data.clear();
        hex(action_t->exchange_fee_address, std::back_inserter(hex_data));
        op_return_data += hex_data;

    }
    else if (action->operation_code == ActionObject::T4)
    {
        NCActT action_t =(NCActT)action;
        std::string hex_data;
        hex(action_t->asset_type, std::back_inserter(hex_data));
        op_return_data += hex_data;
        // hex_data.clear();
        // hex(action_t->asset_id, std::back_inserter(hex_data));
        op_return_data += action_t->asset_id;
        hex_data.clear();
        LongToBytes(action_t->party1_token_qty,hex_data);
        op_return_data += hex_data;
        hex_data.clear();
        LongToBytes(action_t->party2_token_qty,hex_data);
        op_return_data += hex_data;
        hex_data.clear();
        hex(action_t->timestamp, std::back_inserter(hex_data));
        op_return_data += hex_data;
        hex_data.clear();
    }

    return true;
}
