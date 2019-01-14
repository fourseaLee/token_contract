#include "txparse.h"

TxParse::TxParse()
{
    tx_obj_ = new TxObject();
}

TxParse::~TxParse()
{
    delete tx_obj_;
}

void TxParse::InitTxObj(const std::string &txid, const std::string &content, uint32_t vout, uint64_t block_height)
{
    tx_obj_->SetTxid(txid);
    tx_obj_->SetOpReturnData(content);
    tx_obj_->SetTxVout(vout);
    tx_obj_->SetBlockHeight(block_height);
}

