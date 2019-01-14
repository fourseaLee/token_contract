#include "init.h"
#include <iostream>
#include <glog/logging.h>
#include "interactive/tokeninteractive.h"
#include "contract/contractobject.h"
#include "contract/designcontract.h"
#include "contract/nchaincontract.h"

int main(int argc, char *argv[])
{
    if ( !token_interactive::ParseCmd(argc,argv) )
    {
        std::cout << "parse cmd error ,--help  show help " << std::endl;
        return false;
    }

    if ( !token_init::InitConfigure() )
    {
        std::cout <<  "init  configure file failed ! " << std::endl;
        return 0;
    }

    if ( !token_init:: InitLog() )
    {
        std::cout << "init log  failed! " << std::endl;
        return 0;
    }
    DesignContract* design = new DesignContract();
    DesignContract::UserContractFormat* user_input = new DesignContract::UserContractFormat();
    ContractObject * obj = design->FormatNChainContract(user_input);

    NChainContract* obj_nchain  = (NChainContract*)obj;
    std::vector<NChainTx *> vect_tx;
    vect_tx = obj_nchain->ActionsToBlockChainTxs();
    NChainTx* tx  = vect_tx[0];
    std::string ret ;
    tx->GetTxContent(ret);





    LOG(INFO) << "INFO------------------" ;
    LOG(WARNING) << "WARNIMG-----------------";
    LOG(ERROR) << "ERROR---------------------";
    DLOG(INFO) << "----------------------------TRACE";
   // LOG(FATAL) << "FATAL--------------------";

    google::ShutdownGoogleLogging();
}
