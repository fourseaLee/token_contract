#include "contractparse.h"
#include "contract/contractobject.h"
#include "machinestack.h"
#include <glog/logging.h>
#include "actionobject.h"
ContractParse::ContractParse()
{

}

bool ContractParse::RunContract(const std::string &contract_address, const std::string &contract_content, const std::string &input)
{
    std::string contract_hash;
    ContractObject::GetContractHash(contract_content,contract_hash);
    if(contract_hash != contract_address)
    {
        LOG(ERROR) <<  contract_address << " ADD " << contract_content << "don't match" ;
        return false;
    }

    using namespace bc;
    data_chunk data_input;
    decode_base16(data_input,input);

    int param_length_position =0;
    uint8_t op_code  =0x00;
    bool is_code = true;
    while(true)
    {
        if (param_length_position >= data_input.size())
        {
            break;
        }
        uint8_t lenght  = data_input.at(param_length_position);
        if (lenght == 1)
        {
            op_code = data_input.at(param_length_position+lenght);
            is_code = ActionObject::IsOpCode(op_code);

            if(is_code)
            {
                machine_->PushCode(op_code);
            }
            else
            {
                std::string data = std::to_string(op_code);
                machine_->PushData(data);
            }

        }
        else
        {
            std::string data ;
            for ( uint8_t i = 0; i < lenght; i++ )
            {
                op_code = data_input.at(param_length_position+lenght);
                data += std::to_string(op_code);
            }
            machine_->PushData(data);

            param_length_position += lenght;
        }
    }

    if (machine_->RunMachine())
    {
        return false;
    }
    return true;


}
