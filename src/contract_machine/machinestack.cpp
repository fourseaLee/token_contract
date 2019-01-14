#include "machinestack.h"
#include "actionobject.h"
#define CONTRACT_HASH_SIZE 32

MachineStack::MachineStack()
{

}

void MachineStack::PushData(const std::string &data)
{
    data_stack_.push(data);
}

void MachineStack::PushCode(unsigned char code)
{
    code_stack_.push(code);
}

MachineStack::ErrorCode MachineStack::RunMachine()
{
    MachineStack::ErrorCode error_code = OK;
    while(true)
    {
        if(code_stack_.size() == 0)
            break;
        unsigned char op_code = code_stack_.top();
        code_stack_.pop();
        switch (op_code)
        {
            case ActionObject::C1:
                if(code_stack_.size() !=0  || data_stack_.size() != 1)
                {
                    error_code = FORMAT_ERROR;
                }
                else
                {
                    std::string data  = data_stack_.top();
                    data_stack_.pop();
                    if (data.size() != CONTRACT_HASH_SIZE)
                    {
                        error_code = FORMAT_ERROR;
                        break;
                    }
                }
                break;
            case ActionObject::C2:
            case ActionObject::C3:
            case ActionObject::A1:
            case ActionObject::A2:
            case ActionObject::A3:
            case ActionObject::T1:
            case ActionObject::T2:
            case ActionObject::T3:
            case ActionObject::T4:
            case ActionObject::G1:
            case ActionObject::G2:
            case ActionObject::G3:
            case ActionObject::G4:
            case ActionObject::G5:
            case ActionObject::G6:
            case ActionObject::E1:
            case ActionObject::E2:
            case ActionObject::E3:
            case ActionObject::E4:
            case ActionObject::E5:
            case ActionObject::M1:
            case ActionObject::M2:
            case ActionObject::R1:
            case ActionObject::R2:
            case ActionObject::R3:
            case ActionObject::R4:
               break;
            default:
                break;
        }
        if(OK != error_code)
            break;

    }


    return error_code;
}
