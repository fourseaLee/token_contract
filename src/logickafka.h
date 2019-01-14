#ifndef LOGICKAFKA_H
#define LOGICKAFKA_H
#include "logic.h"

class LogicKafka : public Logic
{
public:
    LogicKafka();
    ~LogicKafka();
public:
    bool ExchangeSign(const json& json_contract);

    bool PushSign(const json& json_contract, std::string &txid);

protected:
    bool CreateUnSignTransaction(Tx *tx, json &utxo, double utxo_currency, 
            const std::string &utxo_address, std::string &tx_hex);

private:
    token_interactive::CurlParams* contract_curl_params_;

};

#endif // LOGICKAFKA_H
