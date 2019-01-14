#ifndef CONTRACTPARSE_H
#define CONTRACTPARSE_H
#include <string>

class MachineStack;
class ContractParse
{
public:
    ContractParse();


    bool RunContract(const std::string& contract_address,const std::string& contract_content, const std::string&input);


protected:

    MachineStack* machine_;

};

#endif // CONTRACTPARSE_H
