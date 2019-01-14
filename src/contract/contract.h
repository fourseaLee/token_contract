#ifndef CONTRACT_H
#define CONTRACT_H
#include <string>
#include "contractobject.h"
#include "contract_machine/action.h"
#include <vector>
#include "transaction/tx.h"

typedef  Action::Operation*     NCAct;
typedef  Action::ContractOperation*   NCActC;
typedef  Action::AssetOperation*      NCActA;
typedef  Action::TransferOperation*   NCActT;

class Contract : public ContractObject
{
public:
    Contract();
    ~Contract();
    enum ContractType
    {
        UNDEFINE_CONTRACT  = 0X00,
        SHC_CREATE  = 0x01,
        SHC_TRANSFER = 0x02
    };

    void SetContractAddress(const std::string& contract_address);

    void GetContractAddress(std::string& contract_address);

    void SetTokenSendAddress(const std::string& token_send_address);

    void GetTokenSendAddress( std::string& token_send_address);

    void SetTokenRecieveAddress(const std::string& token_recieve_address);

    void GetTokenRecieveAddress( std::string& token_recieve_address);

    void SetBchCountT2(double bch_count_T2);

    void GetBchCountT2(double& bch_count_T2);

    void PushAction(NCAct contact_action);

    std::vector<Tx* > ActionsToBlockChainTxs();

    void SetContractType(ContractType contract_type );
    int  GetContractType();

    void SetIssuerAddress(const std::string& issuer_address);

    void GetIssuerAddress(std::string &issuer_address);
public:
    static void FloatToBytes(const float float_value, std::string& hex_data);
    static void BytesToFloat(float& float_value, const std::string& hex_data);
    static void LongToBytes(const uint64_t long_value, std::string& hex_data);
    static void BytesToLong(uint64_t& long_value, const std::string& hex_data);
    static void ShortToBytes(const uint16_t short_value, std::string& hex_data);
    static void BytesToShort(uint16_t& short_value, const std::string& hex_data);

protected:
    bool SHCCreate(std::vector<Tx* >& vect_tx);

    bool SHCTransfer(std::vector<Tx* >& vect_tx);
public:
    bool FormatOpReturnData(NCAct action, std::string& op_return_data);
protected:
    std::string contract_address_;
    std::vector<NCAct> vect_contract_actions_;
    std::string token_send_address_;
    std::string token_recieve_address_;
    double bch_count_T2_;
    int contract_type_;
    std::string issuer_address_;
};

#endif // CONTRACT_H
