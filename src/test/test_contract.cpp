#include "testingsetup.h"
#include "contract/contractobject.h"
#include <iostream>
#include <string>
#include <sstream>
#include <boost/algorithm/hex.hpp>

BOOST_AUTO_TEST_SUITE(contract)

BOOST_AUTO_TEST_CASE(standard)
{
    ContractObject contract_obj;
    std::string priv_key;
    std::string pub_key;
    ContractObject::GenerateEncryptKey(priv_key,pub_key);
    contract_obj.SetPrivateKey(priv_key);
    std::cout << "private key : " << priv_key << std::endl;
    std::cout << "public key :" << pub_key  << std::endl;

    std::cout << std::endl << std::endl << std::endl;
    std::string contract_content = "asdsdfasdfasfeafaeffasdfafjoiqweirujp9fjahfsasfdhuiq2fbhalfhadfjhqwiuuhiasfhafhjalsddjlvkhajscbnlalasdhfjashdfjashdflashdfjkasdhfjaskfhasjklfdhhalsasdfhweliuasdjfhasjdlfhasjdkfhasdfjasdhfjkasdhfjksdahfjkasdfhdlasfdjkafhjaksfdhaksjlfhasjdfhasjkdfhaskjfsaklfdhasjkhjdjahdjadfhajsdfhajdfhaufiehpqeobhvbanzhahiaskdfhjashdfuebnanxdufiazuehlkanzhuiwebnakdsgjad;dfeianfkadfalweiuhjfiuasflkajshdfklehfuiasdfheuifhueifuhiwqefhuasfhlakhfueinfahjkldshfasdnalgjhalkdsjncubealbhganasdfhasglkjasdhuiebnakjhadfaljdkfhadsdjlfkafklasdfhkjhfdsakljfhasjkldsadfhasdjkfhsadjkfhsajkfhsakkksadhfakshdfasjkfhiueiasdfhkadlsfasjhdfuieufadusdhfklasdfheklfhasulidfhlkasdhfaslkjdfhasdkljfhasfdkljashfkajsdhfadfaskfhuiefhasdlkfasjdfhuefbgjklasdhglaskdhgailusfyheiofhasifdupsfduheuabsdlfhasjdfheufbsadlfahsdufheufaslfdbxnbqywgavyqwyausdighasdgkhasjdghasidnbfiweufaishdifasgnasidufhqwehasdkjfkasdfhquiweiafshabkqasfdlasdhfoahelfahsdflfhlqweuifhauifhasldfjhweufqwuiefhiuwehfuihfiusahfuieheriqhuiwhfnaishdfkjasdhfuiweqnhakdshfkjashdfwueihasdifheuihasdnlmcbxahznqiqr6526596fawefaef";
    std::string contract_hash ;
    ContractObject::GetContractHash(contract_content,contract_hash);
    std::cout << "hash contract: " << contract_hash << std::endl;

    std::string sign_contract;
    bool ret = contract_obj.SignContract(contract_content,sign_contract);   
    if (ret)
        std::cout  << "sign succcess: " << sign_contract <<std::endl; 
    BOOST_CHECK(ret);
    contract_obj.SetPublicKey(pub_key);
    ret = contract_obj.VerifySignature(sign_contract,contract_content);
    BOOST_CHECK(ret);

}
void FloatToByte(float floatNum, char* byteArry)////浮点数到十六进制转换2
{
    char* pchar=(char*)&floatNum;
    for(int i=0;i<sizeof(float);i++)
    {
        *byteArry=*pchar;
        pchar++;
        byteArry++;

    }
}

void BignumToByte(uint64_t bigNum, char* byteArry)////浮点数到十六进制转换2
{
    char* pchar=(char*)&bigNum;
    for(int i=0;i<sizeof(uint64_t);i++)
    {
        *byteArry=*pchar;
        pchar++;
        byteArry++;

    }
}

void LongToBytes(uint64_t long_value, std::string& hex_data)
{
    unsigned char byte_array[8];
    char* pchar=(char*)&long_value;
    for(int i=0;i<sizeof(uint64_t);i++)
    {
        pchar++;
        byte_array[i] = *pchar;
    }
    using namespace bc;
    data_chunk data_bignum;
    for (int i=0; i < 8; i++)
    {
        data_bignum.push_back(byte_array[i]);
    }
    hex_data = encode_base16(data_bignum);

}

BOOST_AUTO_TEST_CASE(encode)
{
    using namespace boost::algorithm;
    std::string GoverningLaw = "AUS";
    std::cout << GoverningLaw << std::endl;
    std::string hex_data;
    hex(GoverningLaw, std::back_inserter(hex_data));
    std::cout << GoverningLaw << std::endl;
    using namespace bc;
    data_chunk data_law ;
    decode_base16(data_law,hex_data);
    std::cout << "law size: " << data_law.size() << std::endl;
    std::string output = encode_base16(data_law);
    std::cout << "law content: "<< output << std::endl;

    std::string base58_org  = "quick/block/nchain/asset/id";
    
    float t = 0.02;
    char float_to_hex[4];
    data_chunk data_float;
    FloatToByte(t,float_to_hex);
    for (int i=0; i < 4; i++)
    {
        data_float.push_back(float_to_hex[i]);
    }

    std::cout << encode_base16(data_float) << std::endl;
    std::cout << std::endl;
    float out_float = *((float*)float_to_hex);//
    std::cout << "out float: " << out_float << std::endl;

    std::string data = encode_base16(data_float);
    std::cout << "data: " << data << std::endl;

    data_chunk decode_data;
    decode_base16(decode_data,data);
    char hex_to_float[4];
    for(int i = 0; i < 4; ++i)
    {
        hex_to_float[i] = decode_data[i];
    }

    float out_float1 = *((float*)hex_to_float);//
    std::cout << "out float1: " << out_float1 << std::endl;
    uint64_t r = 1000000000000;

    char big_to_hex[8];
    data_chunk data_bignum;
    BignumToByte(r,big_to_hex);
    for (int i=0; i < 8; i++)
    {
        data_bignum.push_back(big_to_hex[i]);
    }
    std::cout << encode_base16(data_bignum) << std::endl;
    std::cout << std::endl;
    uint64_t l = 128934843;
    std::string encode_data;
    LongToBytes(l,encode_data);
    std::cout << "encode :" << encode_data << std::endl;
    std::string sub_pre = "00000000001201";
    std::cout << "prefix: " << sub_pre.substr(0,8) << std::endl;
    std::cout << "action: "  << sub_pre.substr(8,4) << std::endl;
    std::cout <<  "version: " << sub_pre.substr(12,2) << std::endl;

}


BOOST_AUTO_TEST_CASE(address)
{
    ContractObject contract_obj;
    std::string address;
    ContractObject::ChainNet chain_net  = ContractObject::REGTESTNET;
    contract_obj.CreateNewAddress(address,chain_net);
    std::cout << "regtest new address: " << address << std::endl;
    std::string priv_key_base58;
    contract_obj.GetPrivateKeyBase58(priv_key_base58);
    std::cout << "regtest privkey: " << priv_key_base58 << std::endl;

    chain_net = ContractObject::MAINNET;
    contract_obj.CreateNewAddress(address,chain_net);
    std::cout << "main new address: " << address << std::endl; 
    contract_obj.GetPrivateKeyBase58(priv_key_base58);
    std::cout << "regtest privkey: " << priv_key_base58 << std::endl;
}

BOOST_AUTO_TEST_CASE(signHash)
{
    ContractObject contract_obj;
    std::string hash_hex = "e306c5cd92c9777037f1db94df9a2437997bd9356fa20ccf59abb246407456b5";
    contract_obj.SignHash("cTyU64Dv1dBafgAHxegWACKgxuHKytXXVXSwusqk1M7DgvC8Cymh",hash_hex);
    hash_hex = "b556744046b2ab59cf0ca26f35d97b9937249adf94dbf1377077c992cdc506e3"; 
    contract_obj.SignHash("cTyU64Dv1dBafgAHxegWACKgxuHKytXXVXSwusqk1M7DgvC8Cymh",hash_hex);
	std::cout << hash_hex<< std::endl;
}
BOOST_AUTO_TEST_CASE(signTx)
{
    ContractObject contract_obj;
//bitcoin-cli  --regtest createrawtransaction "[{\"txid\":\"a041122353ee4b070c2278c4250c43f606641910457c61a693843ea6d15d244c\",\"vout\":1}]" "{\"mvUEVLwVkystigAPsZqirksQtfVyJ3gun7\":0.99948}"    
    ContractObject::BuildTx tx;
    ContractObject::BuildTxVin tx_vin;
    tx_vin.txid = "a041122353ee4b070c2278c4250c43f606641910457c61a693843ea6d15d244c";
    tx_vin.vin_index = 1;
    tx_vin.value = "0.99949";
    tx.opreturn_data = "1234567890";
    tx.vect_vin.push_back(tx_vin);
//    tx.map_vin["a041122353ee4b070c2278c4250c43f606641910457c61a693843ea6d15d244c"] = 1;
    tx.map_vout["mvUEVLwVkystigAPsZqirksQtfVyJ3gun7"] = "0.99948";
    tx.priv_key_base58 = "cTyU64Dv1dBafgAHxegWACKgxuHKytXXVXSwusqk1M7DgvC8Cymh";
    std::string sirialize_tx;
    contract_obj.CreateTransaction(tx,sirialize_tx);
    std::cout << " sirialize tx1  : " << sirialize_tx << std::endl;


    tx.opreturn_data = "30303030333231534843954d5a49fd70d9b8bcdb35d252267829957f7ef7fa6c74f88419bdc5e82209f4a0860100000000";
	contract_obj.CreateTransaction(tx,sirialize_tx);
    std::cout << " sirialize tx2 : " << sirialize_tx << std::endl;


    tx.opreturn_data = "30303030333231534843954d5a49fd70d9b8bcdb35d252267829957f7ef7fa6c74f88419bdc5e82209f4a0860100000000003230313830323032424348cdcc4c3ecdcc4c3e71716479717530786E33716B6D7777753334743966787267367A77716D6D73";
	contract_obj.CreateTransaction(tx,sirialize_tx);
    std::cout << " sirialize tx3 : " << sirialize_tx << std::endl;
}

BOOST_AUTO_TEST_CASE(EncodeAddress)
{
    ContractObject contract_obj;
	std::string address;
	contract_obj.GetAddressByPrivKeyBase58("cS7KPCm1zJYwaNUo6sAyxHgadvyRnmWTxxXjBbwoGVihcun8yJrD", ContractObject::REGTESTNET ,address);
	std::cout << "cS7KPCm1zJYwaNUo6sAyxHgadvyRnmWTxxXjBbwoGVihcun8yJrD: \n" << address << std::endl;
	contract_obj.GetAddressByPrivKeyBase58("cTyU64Dv1dBafgAHxegWACKgxuHKytXXVXSwusqk1M7DgvC8Cymh", ContractObject::REGTESTNET ,address);
	std::cout << "cTyU64Dv1dBafgAHxegWACKgxuHKytXXVXSwusqk1M7DgvC8Cymh: \n" << address << std::endl;
	contract_obj.GetAddressByPrivKeyBase58("cSC5hn64977UMZX28jVyqYPhMh81Z7vQqvnGyhfVvHHY2oSkq3Um", ContractObject::REGTESTNET ,address);
	std::cout << "cSC5hn64977UMZX28jVyqYPhMh81Z7vQqvnGyhfVvHHY2oSkq3Um: \n" << address << std::endl;
	contract_obj.GetAddressByPrivKeyBase58("cSBrK8e1fyJwur2PFU4uVKaiCdoVAd9iHHn3EQ4VKA49Qx223U5S", ContractObject::REGTESTNET ,address);
	std::cout << "cSBrK8e1fyJwur2PFU4uVKaiCdoVAd9iHHn3EQ4VKA49Qx223U5S: \n" << address << std::endl;



}
BOOST_AUTO_TEST_SUITE_END()

