#include "designcontract.h"
#include "contractobject.h"
#include "contract.h"
#include <glog/logging.h>
#include <iostream>

DesignContract::DesignContract()
{

}

ContractObject *DesignContract::FormatContract(const std::string &contract_content)
{
    ContractObject* contract_obj = NULL;
    std::string priv_key;
    std::string pub_key;
    bool  ret = ContractObject::GenerateEncryptKey(priv_key,pub_key);
    if (!ret)
        return contract_obj;
    contract_obj = new ContractObject(priv_key,pub_key,contract_content);
    return contract_obj;
}

ContractObject *DesignContract::FormatContract(DesignContract::UserContractFormat *user_input_contract)
{
    using namespace bc;
    ContractObject*contract_obj = NULL;
    contract_obj  = new Contract();
    ((Contract*)contract_obj)->SetContractType(Contract::SHC_CREATE);
    ((Contract*)contract_obj)->SetIssuerAddress(user_input_contract->issuer_address);
    ((Contract*)contract_obj)->SetContractAddress(user_input_contract->contract_address);

    NCAct contract_action = new Action::ContractOperation();
    NCActC contract_action_c =  (NCActC)contract_action;
    contract_action_c->operation_code = Action::C1;

    contract_action_c->contract_name = user_input_contract->contract_name;
    contract_action_c->contract_file_hash = user_input_contract->contract_content;
    ((Contract*)contract_obj)->PushAction(contract_action);

    contract_action = NULL;
    contract_action = new Action::ContractOperation();
    contract_action_c = (NCActC)contract_action;
    contract_action_c->operation_code = Action::C2;

    contract_action_c->contract_name = user_input_contract->contract_name;
    contract_action_c->contract_file_hash = user_input_contract->contract_content;
    ((Contract*)contract_obj)->PushAction(contract_action);


    contract_action = new Action::AssetOperation();
    NCActA contract_action_a = (NCActA)contract_action;
    contract_action_a->operation_code = Action::A1;
    contract_action_a->asset_type = "SHC";// user_input_contract->asset_type;
    contract_action_a->asset_id = user_input_contract->asset_id;
    contract_action_a->qty = user_input_contract->asset_qty;
    contract_action_a->contract_fee_currency = "BCH";
    contract_action_a->contract_fee_var = 0.01;
    contract_action_a->contract_fee_fixed = 15;

    Payload* payload = new SHC();
    payload->operation_code = Action::A1;
    contract_action_a->payload = FormatPayload(payload);
    delete payload;
    payload = NULL;
    ((Contract*)contract_obj)->PushAction(contract_action);

    contract_action = NULL;
    contract_action = new Action::AssetOperation();
    contract_action_a = (NCActA)contract_action;
    contract_action_a->operation_code = Action::A2;
    contract_action_a->asset_type =  "SHC"; // user_input_contract->asset_type;
    contract_action_a->asset_id = user_input_contract->asset_id;
    contract_action_a->asset_revision = 32;
    contract_action_a->voting_system = "M";
    contract_action_a->qty = user_input_contract->asset_qty;
    contract_action_a->contract_fee_currency = "BCH";
    contract_action_a->contract_fee_var = 0.01;
    contract_action_a->contract_fee_fixed = 15;
    payload = new SHC();
    payload->operation_code = Action::A1;
    contract_action_a->payload = FormatPayload(payload);
    delete payload;
    ((Contract*)contract_obj)->PushAction(contract_action);

    return contract_obj;
}

ContractObject* DesignContract::TransferShares(DesignContract::TransferSHC *transfer_SHC)
{
    using namespace bc;
    ContractObject*contract_obj = NULL;
    contract_obj  = new Contract();
    ((Contract*)contract_obj)->SetContractType(Contract::SHC_TRANSFER);
    ((Contract*)contract_obj)->SetContractAddress(transfer_SHC->contract_address);
    ((Contract*)contract_obj)->SetBchCountT2(transfer_SHC->currency_count);
    ((Contract*)contract_obj)->SetTokenSendAddress(transfer_SHC->send_address);
    ((Contract*)contract_obj)->SetTokenRecieveAddress(transfer_SHC->recieve_address);

    NCAct contract_action = new Action::TransferOperation();
    NCActT contract_action_t =  (NCActT)contract_action;
    contract_action_t->operation_code = Action::T2;
    contract_action_t->party1_asset_type = "SHC"; // transfer_SHC->asset_type;
    contract_action_t->party1_asset_id = transfer_SHC->asset_id;
    contract_action_t->party1_token_qty = transfer_SHC->share_count;
    contract_action_t->timestamp = "20181230";
    contract_action_t->exchange_fee_currency = "BCH";

    contract_action_t->exchange_fee_var = 0.2;
    contract_action_t->exchange_fee_fixed = 3.8;
    contract_action_t->exchange_fee_address  = transfer_SHC->contract_address;
    ((Contract*)contract_obj)->PushAction(contract_action);

    contract_action = NULL;
    contract_action = new Action::TransferOperation();
    contract_action_t =  (NCActT)contract_action;
    contract_action_t->operation_code = Action::T4;
    contract_action_t->asset_type = transfer_SHC->asset_type;
    contract_action_t->asset_id = transfer_SHC->asset_id;
    contract_action_t->party1_token_qty = transfer_SHC->send_remain;
    contract_action_t->party2_token_qty = transfer_SHC->recieve_remain;
    ((Contract*)contract_obj)->PushAction(contract_action);

    return contract_obj;
}

std::string DesignContract::FormatPayload(DesignContract::Payload *payload)
{
    std::string ret ;

    return ret;

}
