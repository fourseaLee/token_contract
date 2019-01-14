#ifndef TOKENOBJECT_H
#define TOKENOBJECT_H
#include <string>

class ActionObject
{
public:
    ActionObject();
    enum ContractCode
    {
        ERROR_CODE = 0x00,
        //Contract Operations
        C1 = 0x11,
        C2 = 0x12,
        C3 = 0x13,

        //Asset  Operations
        A1 = 0x21,
        A2 = 0x22,
        A3 = 0x23,

        //Transfer  Operations
        T1 = 0x31,
        T2 = 0x32,
        T3 = 0x33,
        T4 = 0x34,

        //Govemance  Operations
        G1 = 0x41,
        G2 = 0x42,
        G3 = 0x43,
        G4 = 0x44,
        G5 = 0x45,
        G6 = 0x46,

        //Enforcement  Operations
        E1 = 0x51,
        E2 = 0x52,
        E3 = 0x53,
        E4 = 0x54,
        E5 = 0x55,

        //Messaging  Operations
        M1 = 0x61,
        M2 = 0x62,

        //Registry  Operations
        R1 = 0x71,
        R2 = 0x72,
        R3 = 0x73,
        R4 = 0x74
    };
    static bool IsOpCode(unsigned char op_code);
    static std::string CodeSerialize(ContractCode op_code);
    static ContractCode CodeUnSerialize(const std::string& code_serialize);
};

#endif // ACTIONOBJECT_H
