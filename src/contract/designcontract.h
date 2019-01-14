#ifndef DESIGNCONTRACT_H
#define DESIGNCONTRACT_H
#include <string>

//this class is the contract design , format various  constract standard
class ContractObject;
class DesignContract
{
public:
    DesignContract();

    ContractObject* FormatContract(const std::string& contract_content);
    struct UserContractFormat
    {
        std::string issuer_address;
        std::string contract_address;
        std::string contract_content;
        std::string contract_name;
        std::string asset_type;
        std::string asset_id;
        uint64_t asset_qty;

    };


    ContractObject* FormatContract(UserContractFormat* user_input_contract);

    struct Payload
    {
        unsigned char operation_code;
        unsigned char version;//size 1
        std::string trading_restricion;//size 3
        Payload()
        {}
        virtual ~Payload()
        {}
    };

    struct SHC:public Payload
    {
        unsigned char dividend_type; //size 1 (C: Combination V: Variable F:Fixed)
        float dividend_fixed ;//size 4

        //size 1 (Q: Quarterly M: Monthly W: Weekly A: Annuall B: Biannually)
        unsigned char disttribution_interval;
        unsigned char guaranteed; //size 1 (Y or N)
        std::string ticker; //size 4
        std::string ISIN; //size 12
        std::string description; //size 113
        SHC()
        {

        }
        virtual ~SHC()
        {

        }

    };

    struct TransferSHC
    {
        std::string asset_type;
        std::string asset_id;
        std::string send_address;
        std::string recieve_address;
        std::string contract_address;
        double currency_count;
        uint64_t share_count;
        uint64_t send_remain;
        uint64_t recieve_remain;
    };

    ContractObject* TransferShares(TransferSHC* transfer_SHC);



protected:
    std::string FormatPayload(Payload* payload);
};

#endif // DESIGNCONTRACT_H
