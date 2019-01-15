#ifndef TXOBJECT_H
#define TXOBJECT_H
#include <string>


class TxObject
{
public:
    TxObject();
    virtual ~TxObject()
    {}

    void SetTxid(const std::string& txid);
    void GetTxid(std::string& txid);

    void SetOpReturnData(const std::string& tx_content);
    void GetOpReturnData(std::string& tx_content);

    void SetTxVout(uint32_t vout);
    void GetTxVout(uint32_t& vout);

    void SetBlockHeight(uint64_t block_height);
    void GetBlockHeight(uint64_t& block_height);

protected:
    std::string txid_;
    std::string op_return_content_;
    uint32_t vout_;
    uint64_t block_height_;

};

#endif // TXOBJECT_H
