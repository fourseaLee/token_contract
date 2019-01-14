#ifndef TXPARSE_H
#define TXPARSE_H

#include "transaction/txobject.h"
#include <map>
#include <vector>
#include <string>
class TxParse
{
public:
    TxParse();
    ~TxParse();

    void InitTxObj(const std::string&txid,const std::string& content,uint32_t vout,uint64_t block_height);

protected:
    TxObject* tx_obj_;
    //the relation of tx_obj_:   key'value  "0" is self  , if key'value < 0 is up, if key'value > 0 is down
     std::map<int, std::vector<std::string> > map_tx_relation_;
};

#endif // TXPARSE_H
