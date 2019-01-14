#ifndef MACHINESTACK_H
#define MACHINESTACK_H
#include <stack>
#include <string>

class MachineStack
{
public:
    MachineStack();
    enum ErrorCode
    {
       OK = 0, //no error
       SIGN_ERROR = 1, //sign data error
       FORMAT_ERROR =2

    };

    void PushData(const std::string& data);
    void PushCode(unsigned char code);

    //run the virtual machine ,only if the data and code is right
    ErrorCode RunMachine();

protected:
    std::stack<std::string> data_stack_;
    std::stack<unsigned char > code_stack_;
};

#endif // MACHINESTACK_H
