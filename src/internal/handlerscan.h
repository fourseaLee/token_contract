#ifndef HANDLERSCAN_H
#define HANDLERSCAN_H
#include "internal/handler.h"

class HandlerScan : public Handler
{
public:
    HandlerScan();

public:
    bool ContractFormat(const json& json_contract,std::string& txid);

    bool AssetCreation(const json& json_contract,std::string& txid);

    bool SettlementToken(const json& json_contract,std::string& txid);
protected:
    bool CreateTransactionT4(Tx *tx, const  std::string &txid_input,  std::string &txid_output);
};

#endif // HandlerScan_H
