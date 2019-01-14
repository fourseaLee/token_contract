#ifndef LOGICPARSETX_H
#define LOGICPARSETX_H
#include "logic.h"

class LogicParseTx : public Logic
{
public:
    LogicParseTx();


public:
    bool ContractFormat(const json& json_contract,std::string& txid);

    bool AssetCreation(const json& json_contract,std::string& txid);

    bool SettlementToken(const json& json_contract,std::string& txid);
protected:
    bool CreateTransactionT4(Tx *tx, const  std::string &txid_input,  std::string &txid_output);
};

#endif // LOGICPARSETX_H
