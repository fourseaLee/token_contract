#ifndef TX_H
#define TX_H
#include "txobject.h"
#include <vector>
#include <map>

class Tx : public TxObject
{
public:
    Tx();
    virtual ~Tx()
    {}

    void SetInputAddress(const std::vector<std::string>& vect_address_input);
    void SetOutputAddress(const std::vector<std::string>& vect_address_output);
    void SetContracAddress(const std::string& contract_address);
    void GetInputAddress(std::vector<std::string>& vect_address_input);
    void GetOutputAddress(std::vector<std::string>& vect_address_output);

    void PushInputAddress(const std::string& address_input);
    void PushOutputAddress(const std::string& address_output);

    std::map<std::string,double> map_input_amout_;
    std::map<std::string,double> map_output_amount_;
protected:
    std::string contract_address_;
    bool up_to_hain_;
    std::vector<std::string> vect_address_input_;
    std::vector<std::string> vect_address_output_;
};

#endif // TX_H
