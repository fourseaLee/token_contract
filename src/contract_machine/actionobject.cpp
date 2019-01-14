#include "actionobject.h"

ActionObject::ActionObject()
{

}

bool ActionObject::IsOpCode(unsigned char op_code)
{
    bool ret = false;
    switch (op_code)
    {
    case C1:
    case C2:
    case C3:
    case A1:
    case A2:
    case A3:
    case T1:
    case T2:
    case T3:
    case T4:
    case G1:
    case G2:
    case G3:
    case G4:
    case G5:
    case G6:
    case E1:
    case E2:
    case E3:
    case E4:
    case E5:
    case M1:
    case M2:
    case R1:
    case R2:
    case R3:
    case R4:
        ret = true;
       break;
    default:
        break;
    }
    return ret;
}

std::string ActionObject::CodeSerialize(ActionObject::ContractCode op_code)
{
    std::string ret ;
    switch (op_code)
    {
    case C1:
        ret = "11";
        break;
    case C2:
        ret = "12";
        break;
    case C3:
        ret = "13";
        break;
    case A1:
        ret = "21";
        break;
    case A2:
        ret = "22";
        break;
    case A3:
        ret = "23";
        break;
    case T1:
        ret = "31";
        break;
    case T2:
        ret = "32";
        break;
    case T3:
        ret = "33";
        break;
    case T4:
        ret = "34";
        break;
    case G1:
        ret = "41";
        break;
    case G2:
        ret = "42";
        break;
    case G3:
        ret = "43";
        break;
    case G4:
        ret = "44";
        break;
    case G5:
        ret = "45";
        break;
    case G6:
        ret = "46";
        break;
    case E1:
        ret = "51";
        break;
    case E2:
        ret = "52";
        break;
    case E3:
        ret = "53";
        break;
    case E4:
        ret = "54";
        break;
    case E5:
        ret = "55";
        break;
    case M1:
        ret = "61";
        break;
    case M2:
        ret = "62";
        break;
    case R1:
        ret = "71";
        break;
    case R2:
        ret = "72";
        break;
    case R3:
        ret = "73";
        break;
    case R4:
        ret = "74";
        break;
    default:
        break;
    }
    return ret;

}

ActionObject::ContractCode ActionObject::CodeUnSerialize(const std::__cxx11::string &code_serialize)
{
    if (code_serialize == "11")
    {
        return C1;
    }
    else if (code_serialize == "12")
    {
        return C2;
    }
    else if (code_serialize == "13")
    {
        return C3;
    }
    else if (code_serialize == "21")
    {
        return A1;
    }
    else if (code_serialize == "22")
    {
        return A2;
    }
    else if (code_serialize == "23")
    {
        return A3;
    }
    else if (code_serialize == "31")
    {
        return T1;
    }
    else if (code_serialize == "32")
    {
        return T2;
    }
    else if (code_serialize == "33")
    {
        return T3;
    }
    else if (code_serialize == "34")
    {
        return T4;
    }
    else if (code_serialize == "41")
    {
        return G1;
    }
    else if (code_serialize == "42")
    {
        return G2;
    }
    else if (code_serialize == "43")
    {
        return G3;
    }
    else if (code_serialize == "44")
    {
        return G4;
    }
    else if (code_serialize == "43")
    {
        return G3;
    }
    else if (code_serialize == "44")
    {
        return G4;
    }
    else if (code_serialize == "45")
    {
        return G5;
    }
    else if (code_serialize == "46")
    {
        return G6;
    }
    else if (code_serialize == "51")
    {
        return E1;
    }
    else if (code_serialize == "52")
    {
        return E2;
    }
    else if (code_serialize == "53")
    {
        return E3;
    }
    else if (code_serialize == "54")
    {
        return E4;
    }
    else if (code_serialize == "55")
    {
        return E5;
    }
    else if (code_serialize == "61")
    {
        return M1;
    }
    else if (code_serialize == "62")
    {
        return M2;
    }
    else if (code_serialize == "71")
    {
        return R1;
    }
    else if (code_serialize == "72")
    {
        return R2;
    }
    else if (code_serialize == "73")
    {
        return R3;
    }
    else if (code_serialize == "74")
    {
        return R4;
    }

    return ERROR_CODE;
}
