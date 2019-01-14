#include "contractobject.h"
#include <glog/logging.h>
#include <iostream>
#include <boost/algorithm/hex.hpp>
#include "transaction/tx.h"
#define UNSIGNSIZE 42

ContractObject::ContractObject()
{

}

ContractObject::ContractObject(const std::string &priv_key, const std::string &pub_key, const std::string &contract_content)
{
    priv_key_  = priv_key;
    pub_key_ = pub_key;
    contract_content_ = contract_content;


}

ContractObject::~ContractObject()
{

}

bool ContractObject::GetContractHash(const std::string &contract_content, std::string& contract_hash)
{
    using namespace bc;
    using namespace boost::algorithm;
    std::string hex_data;
    hex(contract_content, std::back_inserter(hex_data));

    data_chunk data_msg ;
    decode_base16(data_msg,hex_data);
    hash_digest my_hash = bitcoin_hash(data_msg);
    contract_hash = encode_base16(my_hash);

    return true;
}

bool ContractObject::GenerateEncryptKey(std::string &priv_key, std::string &pub_key)
{
    using namespace bc;
    //generate private key
    data_chunk my_entropy(ec_secret_size); //256bits
    pseudo_random_fill(my_entropy);
    auto my_secret = to_array<ec_secret_size>(my_entropy);
    priv_key = encode_base16(my_secret);

    //generate public key
    ec_compressed my_pubkey;
    secret_to_public(my_pubkey, my_secret);
    pub_key = encode_base16(my_pubkey);


    return true;
}

bool ContractObject::SignContract(const std::string contract, std::string &sign_contract)
{
    using namespace bc;

    data_chunk data_contract ;
    decode_base16(data_contract,contract);
    auto my_hash = bitcoin_hash(data_contract);

    // My private & public keys.
    data_chunk my_entropy(ec_secret_size); //256bits
    decode_base16(my_entropy,priv_key_);

    auto my_secret = to_array<ec_secret_size>(my_entropy);

    LOG_IF(INFO, !verify(my_secret)) << priv_key_ << "IS NOT SECRET!";

    ec_compressed my_pubkey;
    secret_to_public(my_pubkey, my_secret);

    // Sign hash of TX with my private key.
    ec_signature my_signature;
    sign(my_signature, my_secret, my_hash);

    // Verify Signature.
    LOG_IF(INFO, !verify_signature(my_pubkey, my_hash, my_signature)) << contract  << "FAIL!";

    // Format signature (r,s) as DER.
    der_signature my_der_signature;
    encode_signature(my_der_signature, my_signature);

    // DER serialisation.
    sign_contract = encode_base16(my_der_signature);
    std::cout << "sign contract: " <<sign_contract << std::endl;

    return verify_signature(my_pubkey, my_hash, my_signature);
}

bool ContractObject::VerifySignature(const std::string &sign_contract,const std::string& org_contract)
{
    using namespace bc;
    data_chunk data_sign ;
    decode_base16(data_sign,sign_contract);

    ec_signature my_signature;
    parse_signature(my_signature,data_sign,false);

    der_signature my_der_signature1;
    encode_signature(my_der_signature1, my_signature);

    data_chunk data_org ;
    decode_base16(data_org,org_contract);
    auto my_hash = bitcoin_hash(data_org);

    // My  public keys.
    data_chunk data_public(ec_compressed_size); //256bits
    decode_base16(data_public,pub_key_);

    auto my_pubkey = to_array<ec_compressed_size>(data_public);

    return verify_signature(my_pubkey, my_hash, my_signature);

}

bool ContractObject::SetPrivateKey(const std::string &priv_key)
{
    priv_key_ = priv_key;
    return true;
}

bool ContractObject::GetPrivateKey(std::string &priv_key) const
{
    priv_key = priv_key_;
    return true;
}

bool ContractObject::SetPublicKey(const std::string &pub_key)
{
    pub_key_ = pub_key;
    return true;
}

bool ContractObject::GetPublicKey(std::string &pub_key) const
{
    pub_key = pub_key_;
    return true;
}

bool ContractObject::CreateNewAddress(std::string &address,ChainNet chain_net)
{

    using namespace bc;
    using namespace wallet;
    //generate private key
    data_chunk my_entropy(ec_secret_size); //256bits
    pseudo_random_fill(my_entropy);
    auto my_secret = to_array<ec_secret_size>(my_entropy);
    priv_key_ = encode_base16(my_secret);
    std::cout << "priv_key: " << priv_key_<< std::endl;

    //generate public key
    ec_compressed my_pubkey;
    secret_to_public(my_pubkey, my_secret);

    pub_key_ = encode_base16(my_pubkey);

    chain_net_ = chain_net;

    auto my_pubkeyhash = bitcoin_short_hash(my_pubkey);

    one_byte addr_prefix;
    one_byte secret_prefix;
    one_byte secret_compressed = { { 0x01 } }; //Omitted if uncompressed

    switch (chain_net)
    {
    case MAINNET:
        addr_prefix = {{0x00}};
        secret_prefix = {{0x80}};
        break;
    case TESTNET:
        addr_prefix = {{0x6f}};
        secret_prefix = {{0xef}};
        break;
    case REGTESTNET:
        addr_prefix = {{0x6f}};
        secret_prefix = {{0xef}};
        break;
    default:
        break;
    }

    data_chunk prefix_pubkey_checksum(to_chunk(addr_prefix));
    extend_data(prefix_pubkey_checksum,my_pubkeyhash);
    append_checksum(prefix_pubkey_checksum);

    // Apply prefix, suffix & append checksum
    auto prefix_secret_comp_checksum = to_chunk(secret_prefix);
    extend_data(prefix_secret_comp_checksum, my_secret);
    extend_data(prefix_secret_comp_checksum, secret_compressed);
    append_checksum(prefix_secret_comp_checksum);

    // WIF (mainnet/compressed)
    priv_key_base58_ = encode_base58(prefix_secret_comp_checksum) ;

    data_chunk base_58_data ;
    decode_base58(base_58_data,priv_key_base58_);

    std::string priv_key_code = encode_base16(base_58_data);
    std::cout << "priv key code " << priv_key_code << std::endl;
    std::string sub_priv_key = priv_key_code.substr(2,ec_secret_size*2);
    std::cout << "sub priv key :" <<sub_priv_key << std::endl;



    address = encode_base58(prefix_pubkey_checksum);
    address_ = address;
    return true;
}

ContractObject::BlockChainData *ContractObject::CreateBlockChainData()
{
    using namespace bc;

    data_chunk data_contract ;
    decode_base16(data_contract,contract_content_);
    auto my_hash = bitcoin_hash(data_contract);

    BlockChainData* block_chain_data = new  BlockChainData();
    block_chain_data->contract_address = encode_base16(my_hash);

    // My private & public keys.
    data_chunk my_entropy(ec_secret_size); //256bits
    decode_base16(my_entropy,priv_key_);
    auto my_secret = to_array<ec_secret_size>(my_entropy);
    ec_compressed my_pubkey;
    secret_to_public(my_pubkey, my_secret);

    // Sign hash of TX with my private key.
    ec_signature my_signature;
    sign(my_signature, my_secret, my_hash);


    // Format signature (r,s) as DER.
    der_signature my_der_signature;
    encode_signature(my_der_signature, my_signature);

    // DER serialisation.
    block_chain_data->contract_sign_content = encode_base16(my_der_signature);
    block_chain_data->contract_pubkey = encode_base16(my_pubkey);

    return block_chain_data;
}

void ContractObject::SetContractContent(const std::string &contract_content)
{
    contract_content_ = contract_content;
}

void ContractObject::GetContractAllowAddress(std::vector<std::string> &vect_allow_address)
{
    /*
     * maybe depent on the contract content and rules
    */
}

void ContractObject::GetPrivateKeyBase58(std::string &priv_key_base58)
{
    priv_key_base58 = priv_key_base58_;
}

bool ContractObject::CreateTransaction(const ContractObject::BuildTx &tx,std::string& serialize_tx)
{
    using namespace bc;
    using namespace wallet;
    using namespace machine; //opcode
    using namespace chain;   //transaction, inputs, outputs, script

    data_chunk base_58_data ;
    decode_base58(base_58_data,tx.priv_key_base58);

    std::string priv_key_code = encode_base16(base_58_data);
    std::string priv_key = priv_key_code.substr(2,ec_secret_size*2);

    data_chunk my_entropy(ec_secret_size); //256bits
    decode_base16(my_entropy,priv_key);
    auto my_secret = to_array<ec_secret_size>(my_entropy);

    ec_compressed my_pubkey;
    secret_to_public(my_pubkey, my_secret);

    std::string address;
    GetAddressByPrivKey(priv_key,REGTESTNET,address);

    std::cout << "address: " << address << std::endl;

    payment_address my_address(address);
    script prevout_script = script::to_pay_key_hash_pattern(my_address.hash());
    data_chunk data_prevout = prevout_script.to_data(true);
    std::cout << "prevout script: " << encode_base16(data_prevout) << std::endl;;
    
    transaction bc_tx;
    uint32_t version  = 2u;
    bc_tx.set_version(version);

    auto serialized_version = to_little_endian(bc_tx.version());
    std::cout << encode_base16(to_chunk(serialized_version)) << std::endl;

/*    std::map<std::string,int>::const_iterator con_iter = tx.map_vin.begin();
    for(;con_iter != tx.map_vin.end(); con_iter++)
    {
        hash_digest prev_tx_hash;
        decode_hash(prev_tx_hash,con_iter->first);
        output_point uxto_tospend(prev_tx_hash,con_iter->second);
        input input_tx;
        input_tx.set_previous_output(uxto_tospend);
        input_tx.set_sequence(0xffffffff);
        bc_tx.inputs().push_back(input_tx);
    }
 */
    for (unsigned int i = 0; i < tx.vect_vin.size(); i++)
    {
        hash_digest prev_tx_hash;
        decode_hash(prev_tx_hash,tx.vect_vin.at(i).txid);
        output_point uxto_tospend(prev_tx_hash,tx.vect_vin.at(i).vin_index);
        input input_tx;
        input_tx.set_previous_output(uxto_tospend);
        input_tx.set_sequence(0xffffffff);
        bc_tx.inputs().push_back(input_tx);

    }


    std::map<std::string,std::string>::const_iterator con_iter_out = tx.map_vout.begin();
    uint64_t output_value = 0;
    for(; con_iter_out != tx.map_vout.end(); con_iter_out++)
    {
        payment_address my_address(con_iter_out->first);
        operation::list locking_script = script::to_pay_key_hash_pattern(my_address.hash());

        uint64_t  satoshi_amount = 0;
        decode_base10(satoshi_amount,con_iter_out->second,btc_decimal_places);
        output output_tx(satoshi_amount,locking_script);
       // std::cout <<  "address: " << con_iter_out->first << "value : " << con_iter_out->second << std::endl;
        output_value = satoshi_amount;
        bc_tx.outputs().push_back(output_tx);
    }
    

    if ( !tx.opreturn_data.empty() )
    {
        data_chunk chunk_opreturn;
        decode_base16(chunk_opreturn, tx.opreturn_data);
        data_slice slice_opreturn = chunk_opreturn;
        //decode_base16(slice_opreturn,"12343344");
        operation::list opreturn_script = script::to_pay_null_data_pattern(slice_opreturn);
        output output_tx(0,opreturn_script);
        bc_tx.outputs().push_back(output_tx);

    }

    std::cout << "not sign transaction : " << encode_base16(bc_tx.to_data()) << std::endl;

/*
    con_iter = tx.map_vin.begin();
    uint8_t vin_index(0u);
    for(;con_iter != tx.map_vin.end(); con_iter++)
    {

        endorsement sig;
        uint8_t input_index(con_iter->second);
        output_value = 99949000;
        std::cout << "index map " << input_index  << std::endl;;
        script::create_endorsement(sig, my_secret, prevout_script, bc_tx,vin_index, 0x41,script_version::zero,output_value);
        operation::list sig_script;
        sig_script.push_back(operation(sig));
        sig_script.push_back(operation(to_chunk(my_pubkey)));
        script input_script(sig_script);

        //bc_tx.inputs()[vin_index].set_script(input_script);
        vin_index ++;
    }
*/
    if (tx.sign_all)
    {
        uint8_t vin_pos(0u);
        for (unsigned int i = 0; i < tx.vect_vin.size(); i++)
        {
            endorsement sig;
           // uint8_t input_index(tx.vect_vin.at(i).vin_index);
            uint64_t vin_value = 0;
            decode_base10(vin_value,tx.vect_vin.at(i).value,btc_decimal_places);

            std::cout << "vin " << i << " : " << vin_value << std::endl;
            script::create_endorsement(sig, my_secret, prevout_script, bc_tx,vin_pos, 0x41,script_version::zero,vin_value);
            operation::list sig_script;
            sig_script.push_back(operation(sig));
            sig_script.push_back(operation(to_chunk(my_pubkey)));
            script input_script(sig_script);

            bc_tx.inputs()[vin_pos].set_script(input_script);
            vin_pos ++;
        }
    }
    else
    {
        for (unsigned int i = 0; i < tx.vect_pos.size(); i++)
        {
            uint8_t vin_pos(tx.vect_pos.at(i));
            endorsement sig;
           // uint8_t input_index(tx.vect_vin.at(vin_pos).vin_index);
            uint64_t vin_value = 0;
            decode_base10(vin_value,tx.vect_vin.at(vin_pos).value,btc_decimal_places);
            bool ret =  script::create_endorsement(sig, my_secret, prevout_script, bc_tx,vin_pos, 0x41,script_version::zero,vin_value);
            if(!ret)
            {
                std::cout << "sign failed: " << tx.vect_vin.at(vin_pos).txid << " : " << tx.vect_vin.at(vin_pos).vin_index << std::endl;
            }
            operation::list sig_script;
            sig_script.push_back(operation(sig));
            sig_script.push_back(operation(to_chunk(my_pubkey)));
            script input_script(sig_script);
            bc_tx.inputs()[vin_pos].set_script(input_script);
        }
    }



    /*witness empty_witness;      // Only verified in a p2w tx.
    uint64_t prevout_amount(1); // Only verified in a p2w endorsement.

    // Verify input script (and endorsement) for input 0.
    auto ec_input0 = script::verify(bc_tx, input0_index,rule_fork::all_rules,
          input_script0, empty_witness, prevout_script0,
          prevout_amount);
    // Success.
    std::cout << ec_input0.message() << std::endl;
    */

    std::string hex_tx  = encode_base16(bc_tx.to_data());
    serialize_tx  = hex_tx;

    std::cout << "sign transaction: " << hex_tx << std::endl;
    return true;
}

bool ContractObject::SignTransaction(const std::string &priv_key_base58,
                                     const std::string &tx_hex,
                                     const std::vector<BuildTxVin> &vect_vin,
                                     std::string &tx_result)

{

    using namespace bc;
    using namespace wallet;
    using namespace machine; //opcode
    using namespace chain;   //transaction, inputs, outputs, script

    data_chunk base_58_data ;
    decode_base58(base_58_data,priv_key_base58);

    std::string priv_key_code = encode_base16(base_58_data);
    std::string priv_key = priv_key_code.substr(2,ec_secret_size*2);

    data_chunk my_entropy(ec_secret_size); //256bits
    decode_base16(my_entropy,priv_key);
    auto my_secret = to_array<ec_secret_size>(my_entropy);

    ec_compressed my_pubkey;
    secret_to_public(my_pubkey, my_secret);

    std::string address;
    GetAddressByPrivKey(priv_key,REGTESTNET,address);

    std::cout << "address: " << address << std::endl;

    payment_address my_address(address);
    script prevout_script = script::to_pay_key_hash_pattern(my_address.hash());

    data_chunk chunk_tx;
    decode_base16(chunk_tx, tx_hex);

    transaction bc_tx = transaction::factory(chunk_tx);
    std::vector<input> vect_input = bc_tx.inputs();

    if (vect_input.size() != vect_vin.size())
    {
        std::cout << "tx input size is : "  << vect_input.size() << std::endl;
        std::cout << "tx vin size is: "  << vect_vin.size() << std::endl;
        std::cout << "Their sizes are not same! " << std::endl;
        return false;
    }

    for (unsigned int i = 0; i < vect_input.size(); i++)
    {
        uint8_t vin_pos(i);
        endorsement sig;
       // uint8_t input_index(tx.vect_vin.at(vin_pos).vin_index);
        uint64_t vin_value = 0;
        if (vect_input.at(i).to_data().size() >= UNSIGNSIZE)
        {
            continue;
        }
        decode_base10(vin_value,vect_vin.at(i).value,btc_decimal_places);
        bool ret = script::create_endorsement(sig, my_secret, prevout_script, bc_tx,vin_pos, 0x41,script_version::zero,vin_value);
        if (!ret)
        {
            continue;
        }

        operation::list sig_script;
        sig_script.push_back(operation(sig));
        sig_script.push_back(operation(to_chunk(my_pubkey)));
        script input_script(sig_script);
        bc_tx.inputs()[vin_pos].set_script(input_script);
    }

    tx_result  = encode_base16(bc_tx.to_data());
    std::cout << "serialize tx : " << tx_result << std::endl;
    return true;
}

bool ContractObject::GetAddressByPrivKey(const std::string &priv_key,ChainNet chain_net, std::string &address)
{
    using namespace bc;
    using namespace wallet;

    // My private & public keys.
    data_chunk my_entropy(ec_secret_size); //256bits
    decode_base16(my_entropy,priv_key);
    auto my_secret = to_array<ec_secret_size>(my_entropy);

    //generate public key
    ec_compressed my_pubkey;
    secret_to_public(my_pubkey, my_secret);
    auto my_pubkeyhash = bitcoin_short_hash(my_pubkey);

    one_byte addr_prefix;

    switch (chain_net)
    {
    case MAINNET:
        addr_prefix = {{0x00}};
        break;
    case TESTNET:
        addr_prefix = {{0x6f}};
        break;
    case REGTESTNET:
        addr_prefix = {{0x6f}};
        break;
    default:
        break;
    }

    data_chunk prefix_pubkey_checksum(to_chunk(addr_prefix));
    extend_data(prefix_pubkey_checksum,my_pubkeyhash);
    append_checksum(prefix_pubkey_checksum);
    address = encode_base58(prefix_pubkey_checksum);
    return true;
}

bool  ContractObject::SignHash(const std::string& priv_key_base58,const std::string& hash_data)
{
    using namespace bc;
    data_chunk base_58_data ;
    decode_base58(base_58_data,priv_key_base58);

    std::string priv_key_code = encode_base16(base_58_data);
    std::string priv_key = priv_key_code.substr(2,ec_secret_size*2);
    data_chunk my_entropy(ec_secret_size); //256bits
    decode_base16(my_entropy,priv_key);
    auto my_secret = to_array<ec_secret_size>(my_entropy);

    hash_digest my_hash;
    decode_hash(my_hash, hash_data);
    std::reverse(std::begin(my_hash), std::end(my_hash));
   // auto my_hash = hash_literal("e306c5cd92c9777037f1db94df9a2437997bd9356fa20ccf59abb246407456b5");//hash_data);

    ec_compressed my_pubkey;
    secret_to_public(my_pubkey, my_secret);

    // Sign hash of TX with my private key.
    ec_signature my_signature;
    sign(my_signature, my_secret, my_hash);

    // Verify Signature.
    // LOG_IF(INFO, !verify_signature(my_pubkey, my_hash, my_signature)) << contract  << "FAIL!";

    // Format signature (r,s) as DER.
    der_signature my_der_signature;
    encode_signature(my_der_signature, my_signature);

    // DER serialisation.
    auto sign_contract = encode_base16(my_der_signature);
    std::cout << "sign contract: " <<sign_contract << std::endl;

    return verify_signature(my_pubkey, my_hash, my_signature);

}

bool ContractObject::GetAddressByPrivKeyBase58(const std::string &priv_key_base58, ContractObject::ChainNet chain_net, std::__cxx11::string &address)
{
    using namespace bc;

    data_chunk base_58_data ;
    decode_base58(base_58_data,priv_key_base58);

    std::string priv_key_code = encode_base16(base_58_data);
    std::string priv_key = priv_key_code.substr(2,ec_secret_size*2);

    data_chunk my_entropy(ec_secret_size); //256bits
    decode_base16(my_entropy,priv_key);
    auto my_secret = to_array<ec_secret_size>(my_entropy);

    ec_compressed my_pubkey;
    secret_to_public(my_pubkey, my_secret);
    GetAddressByPrivKey(priv_key,chain_net,address);
    return true;
}
