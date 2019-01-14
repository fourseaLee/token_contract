#include "tx.h"

Tx::Tx():up_to_hain_(false)
{
   // up_to_hain_ = false;
    vect_address_input_.clear();
    vect_address_output_.clear();

}

void Tx::SetInputAddress(const std::vector<std::string> &vect_address_input)
{
    vect_address_input_ = vect_address_input;
}

void Tx::SetOutputAddress(const std::vector<std::string> &vect_address_output)
{
    vect_address_output_ = vect_address_output;
}

void Tx::SetContracAddress(const std::string &contract_address)
{
    contract_address_ = contract_address;
}

void Tx::GetInputAddress(std::vector<std::string> &vect_address_input)
{
    vect_address_input = vect_address_input_;
}

void Tx::GetOutputAddress(std::vector<std::string> &vect_address_output)
{
    vect_address_output  = vect_address_output_;
}

void Tx::PushInputAddress(const std::string &address_input)
{
    vect_address_input_.push_back(address_input);
}

void Tx::PushOutputAddress(const std::string &address_output)
{
    vect_address_output_.push_back(address_output);
}
