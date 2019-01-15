#ifndef PARSETX_H
#define PARSETX_H
#include "common/json.hpp"
#include "common/util.h"
#include <string>
#include <vector>
#include "contract_machine/action.h"
#include "contract/contract.h"
// Begin to search numbers of top blocks
#define MAX_BLOCK_DEPTH 0
using json = nlohmann::json;
class Tx;
class ParseTx
{
public:
    enum TxVoutType
    {
        ADDRESS = 0,
        OP_RETURN_DATA = 1
    };
    enum TxType
    {
        ONCHAIN = 0,
        INMEMPOOL = 1
    };
    ParseTx();
    ~ParseTx();
public:
    void SetFilterType(TxType filer_type);

    void SetPreBlockHeight(uint64_t pre_block_height);
protected:
    CurlParams* curl_param_;
    TxType filter_type_;
    uint64_t pre_block_height_;
};

class ParseOpreturnData: public ParseTx
{
public:

    ParseOpreturnData(const std::string& url = "192.168.0.68:18332", const std::string& auth = "dev:a");
    virtual ~ParseOpreturnData();

    struct TxVout
    {
        TxVoutType vout_type;
        int  index;
        TxVout():index(-1)
        {
            vout_type = ADDRESS;
        }
        virtual ~TxVout()
        {}
    };

    struct TxVoutAddress:public TxVout
    {
        std::vector<std::string> vect_address;
        double vout_value;
        TxVoutAddress()
        {
            vout_value = 0.0;
        }
        virtual ~TxVoutAddress()
        {
        }
    };

    struct TxVoutOpReturn:public TxVout
    {
        std::string op_return_data;
        TxVoutOpReturn()
        {
        }
        virtual ~TxVoutOpReturn()
        {
        }
    };

    struct TxVin
    {
        TxVout* pre_vout;
        std::string hash;
        int index;
        TxVin()
        {
            index = -1;
            pre_vout = new TxVoutAddress();
        }
        ~TxVin()
        {
            delete pre_vout;
        }
    };

    struct Transaction
    {
        bool coinbase;
        int height;
        // Just storage that include opreturn-payload-massage's vout.
        std::vector<TxVout* > vect_vout;
        std::vector<TxVin* > vect_vin;
        std::string txid;
        Transaction():height(-1)
        {}
        ~Transaction()
        {
            for (unsigned int i = 0; i < vect_vin.size(); i++ )
            {
                delete vect_vin[i];
            }
            for (unsigned int i = 0; i < vect_vout.size(); i++ )
            {
                delete vect_vout[i];
            }
            vect_vin.clear();
            vect_vout.clear();
        }

    };

public:

    void FilterBlock();

    bool FlashToDB();

    bool FlashUtxoToDB();

    bool InitMempoolTx(std::vector<std::string> vect_mempool_tx);
protected:
    bool FlashAction(Transaction *tx, const std::string &op_return_data);

    bool FlashActionT2(Transaction *tx,NCActT action_t);

    bool FlashActionT4(Transaction *tx,NCActT action_t);

protected:

    void filterBlockOnChain();

    void filterBlockMempool();

protected:

    void AddTransaction(const json& json_tx, bool coinbase = false, int block_height = -1);

private:
    std::vector<Transaction* > vect_tx_;
    std::vector<std::string> vect_mempool_tx_;
    std::vector<Transaction* > vect_utxo_;

    Action* action_;

};
#endif //PARSETX_H
