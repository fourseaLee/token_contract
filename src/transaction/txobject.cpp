#include "txobject.h"

TxObject::TxObject()
{

}

void TxObject::SetTxid(const std::string &txid)
{
    txid_ = txid;
}

void TxObject::SetOpReturnData(const std::string &tx_content)
{
    op_return_content_ = tx_content;
}

void TxObject::GetOpReturnData(std::string &tx_content)
{
    tx_content = op_return_content_;
}

void TxObject::SetTxVout(uint32_t vout)
{
    vout_ = vout;
}

void TxObject::SetBlockHeight(uint64_t block_height)
{
    block_height_ = block_height;
}
