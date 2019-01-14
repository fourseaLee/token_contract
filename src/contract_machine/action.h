#ifndef ACTION_H
#define ACTION_H
#include "actionobject.h"
#include <string>

class Action : public ActionObject
{
public:
    Action();

    struct Operation
    {
        ActionObject::ContractCode operation_code;//not serialize into op_return
        std::string symbol; //size 4
        std::string prefix;//size 2
        std::string version;//size 1
        Operation()
        {
            symbol = "0000";
            version = "1";
        }
        virtual ~Operation()
        {}
    };

    struct AssetOperation:public Operation
    {
        std::string asset_type;//size 3
        std::string asset_id; //size 32(base 58)

        //A3, A2
        uint16_t asset_revision;//size 2

        // A2
        std::string timestamp ;//size 8

        std::string authorization_flag; //size 2
        std::string voting_system; //size 1
        std::string vote_multiplier; //size 1
        uint64_t qty;//size 8
        std::string contract_fee_currency; //size 3
        float contract_fee_var;//size 4 (float)
        float contract_fee_fixed; //size 4 (float)
        std::string payload;//size 145
        AssetOperation()
        {
            asset_type  = "SHC";
            asset_id = "1212";
            asset_revision = 2;
            timestamp = "20180808";
            authorization_flag = "MM";
            voting_system = "M";
            vote_multiplier = 'M';
            qty = 100000000000;
            contract_fee_var = 0.04;
            contract_fee_fixed = 4000;
            payload = "HIUAFEIHFAFWEFASDFE";
        }
        virtual ~AssetOperation()
        {

        }
    };

    struct ContractOperation:public Operation
    {
        std::string contract_name;//size 32
        std::string contract_file_hash; //size 32(the hash sha-256 of contract file)
        std::string governing_law;//size 5
        std::string jurisdiction; //size 5

        //C2
        std::string timestamp; //size 8 (time)

        std::string contract_expriration; //size 8(time)
        std::string url; //size 70
        //C2
        std::string contrac_revision;//size 2

        std::string issuer_id;//size 16
        std::string issuer_type; //size 1

        //C2,C3
        std::string contract_operator_id;//size 16

        std::string authorization_flag;//size 2
        std::string voting_system; //size 1
        float initialtive_threshold; //size 4 (float)
        std::string initialtive_threshold_currency;//size 3;
        uint64_t restricted_qty;//size 8
        ContractOperation()
        {
            governing_law = "china";//6368696e61
            jurisdiction = "chlaw";//63686c6177
            timestamp = "20180708";
            contract_expriration = "20180808";
            //68747470733a2f2f7777772e717569636b626c6f636b2e696e666f2f6e636861696e2f746f6b656e697a6174696f6e2f636f6e74726163742f636f6e74656e742f696e666f20
            url = "//https://www.quickblock.info/bchlab/tokenization/contract/content/bch" ;
            contrac_revision = "99";
            issuer_id = "bchlabs_tokenize";//6263686c6162735f746f6b656e697a65;
            issuer_type = "O";//"O" (4f)or "P"
            contract_operator_id = "bchlabs_tokenize";//6263686c6162735f746f6b656e697a65
            authorization_flag = "bc";//6263
            voting_system = "M";//4d
            initialtive_threshold = 0.02;
            initialtive_threshold_currency = "BCH";
            restricted_qty = 1000000000000;
        }
        virtual ~ContractOperation()
        {}
    };

    struct EnforcementOperation:public Operation
    {
        std::string asset_type;//size 3
        std::string asset_id; //size 32(base58)

        //E1
        std::string compliance_action; //size 1
        std::string target_address;//size 34(address)
        std::string deposit_address;//size 34(address)
        std::string supporting_evidence_hash; //size 32(hash SHA-256)

        //E2,E3
        std::string timestamp;//size 8(time)

        uint64_t qty ; //size 8

        //E2,E1
        std::string expriration; //size 8(time)
        std::string message; //size 61

    };

    struct GovernanceOperation:public Operation
    {
        std::string asset_type;//size 3
        std::string asset_id; //size 32(base58)

        //G4,G5,G6
        std::string vote_txn_id;//size 32 (hash SHA-256)

        //G4,G5
        std::string vote;//size 15

        //G1,G2,G3
        std::string vote_type;//size 1
        std::string vote_options;// size 15
        unsigned char vote_max;//size 1
        std::string vote_logic;//size 1
        std::string proposal_description;//size 82
        std::string proposal_document_hash;//size 32 (sha SHA-256)
        std::string vote_cut_off_timestamp;//size 8(time)

        //G3,G5,G6
        std::string timestamp;//size 8(time)

        //G6
        uint64_t option1_tally;//size 8
        uint64_t option2_tally;//size 8
        uint64_t option3_tally;//size 8
        uint64_t option4_tally;//size 8
        uint64_t option5_tally;//size 8
        uint64_t option6_tally;//size 8
        uint64_t option7_tally;//size 8
        uint64_t option8_tally;//size 8
        uint64_t option9_tally;//size 8
        uint64_t option10_tally;//size 8
        uint64_t option11_tally;//size 8
        uint64_t option12_tally;//size 8
        uint64_t option13_tally;//size 8
        uint64_t option14_tally;//size 8
        uint64_t option15_tally;//size 8
        std::string result;//size 16

    };

    struct MessageOperation:public Operation
    {

        std::string timestamp;//size 8

        //M2
        std::string asset_type;//size 3
        std::string asset_id;//size 32(base58)
        std::string rejection_type;//size 1

        //M1
        std::string message_type; //size 2

        std::string message; //size 203
    };

    struct EstablishOperation:public Operation
    {
        //R2,R3
        std::string sublist;//size 4
        std::string KYC;//size 1

        //R1
        std::string registar; //size 16
        std::string register_type; //size 1

        //R1,R2,R3
        std::string KYC_jurisdiction; //size 5
        std::string DOB ;//size 8 (time data of birth ----short name)
        std::string contry_of_residence; //size 3

        std::string supporting_doc_hash ;// size 32 (hash sha-256)
        std::string message; //size 148
    };


    struct TransferOperation:public Operation
    {
        //T4
        std::string asset_type;//size 3
        std::string asset_id;//size 32 (base58)

        //T1
        unsigned int token_qty;//size 8;

        //T2,T3
        std::string party1_asset_type;//size 3
        std::string party1_asset_id; //size 32

        //T2,T3,T4
        uint64_t party1_token_qty;//size 8

        //T2,T3
        std::string offer_valid_until;//size 8(time)

        //T3
        std::string party2_asset_type;//size 3
        std::string party2_asset_id; //size 32

        //T3,T4
        uint64_t party2_token_qty;//size 8

        //T2,T3
        std::string exchange_fee_currency;//size 3
        float exchange_fee_var;//size 4(float %)
        float exchange_fee_fixed;//size 4(float)
        std::string exchange_fee_address;//size 34(address)

        //T4
        std::string timestamp;//size 8 (time)
        TransferOperation()
        {
            exchange_fee_currency = "BCH";
            exchange_fee_var = 0.02;
            exchange_fee_fixed = 15.0;
            asset_type = "SHC";
            timestamp = "20181212";
            offer_valid_until = "20180202";
        }
    };
public:

     Operation* UnSerializeOperation(const std::string& op_return_hex);
protected:
     Operation* NewOperation(const std::string& serialize_code,const std::string &op_return_hex);
     bool UnSerializeAsset(Operation* action,ContractCode op_code, const std::string &op_return_hex);
     bool UnSerializeContract(Operation* action,ContractCode op_code,const std::string &op_return_hex);
     bool UnSerializeTransfer(Operation* action, ContractCode op_code, const std::string &op_return_hex);
};

#endif // ACTION_H
