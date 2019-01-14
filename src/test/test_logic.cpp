#include "testingsetup.h"
#include "contract/contractobject.h"
#include "contract_machine/contractparse.h"
#include "contract_machine/action.h"
#include "contract_machine/assettypes.h"
#include <iostream>
#include <string>

BOOST_AUTO_TEST_SUITE(logic)
//BOOST_AUTO_TEST_CASE(standard)
//{
//    /*
//    ContractObject contract_obj;
//    std::string priv_key;
//    std::string pub_key;
//    ContractObject::GenerateEncryptKey(priv_key,pub_key);
//    contract_obj.SetPrivateKey(priv_key);
//    std::cout << "private key : " << priv_key << std::endl;
//    std::cout << "public key :" << pub_key  << std::endl;
//
//    std::cout << std::endl << std::endl << std::endl;
//   // std::cout << "5df6e0e2761359d3a827558e299fcc381534545f55cf43e41983f5d4c9456" << std::endl;
//    std::string contract_content = "asdsdfasdfasfeafaeffasdfafjoiqweirujp9fjahfsasfdhuiq2fbhalfhadfjhqwiuuhiasfhafhjalsddjlvkhajscbnlalasdhfjashdfjashdflashdfjkasdhfjaskfhasjklfdhhalsasdfhweliuasdjfhasjdlfhasjdkfhasdfjasdhfjkasdhfjksdahfjkasdfhdlasfdjkafhjaksfdhaksjlfhasjdfhasjkdfhaskjfsaklfdhasjkhjdjahdjadfhajsdfhajdfhaufiehpqeobhvbanzhahiaskdfhjashdfuebnanxdufiazuehlkanzhuiwebnakdsgjad;dfeianfkadfalweiuhjfiuasflkajshdfklehfuiasdfheuifhueifuhiwqefhuasfhlakhfueinfahjkldshfasdnalgjhalkdsjncubealbhganasdfhasglkjasdhuiebnakjhadfaljdkfhadsdjlfkafklasdfhkjhfdsakljfhasjkldsadfhasdjkfhsadjkfhsajkfhsakkksadhfakshdfasjkfhiueiasdfhkadlsfasjhdfuieufadusdhfklasdfheklfhasulidfhlkasdhfaslkjdfhasdkljfhasfdkljashfkajsdhfadfaskfhuiefhasdlkfasjdfhuefbgjklasdhglaskdhgailusfyheiofhasifdupsfduheuabsdlfhasjdfheufbsadlfahsdufheufaslfdbxnbqywgavyqwyausdighasdgkhasjdghasidnbfiweufaishdifasgnasidufhqwehasdkjfkasdfhquiweiafshabkqasfdlasdhfoahelfahsdflfhlqweuifhauifhasldfjhweufqwuiefhiuwehfuihfiusahfuieheriqhuiwhfnaishdfkjasdhfuiweqnhakdshfkjashdfwueihasdifheuihasdnlmcbxahznqiqr6526596fawefaef";
//    std::string contract_hash ;
//    ContractObject::GetContractHash(contract_content,contract_hash);
//    std::cout << "hash contract: " << contract_hash << std::endl;
//
//    std::string sign_contract;
//    bool ret = contract_obj.SignContract(contract_content,sign_contract);   
//    if (ret)
//        std::cout  << "sign succcess: " << sign_contract <<std::endl; 
//    BOOST_CHECK(ret);
//    contract_obj.SetPublicKey(pub_key);
//    ret = contract_obj.VerifySignature(sign_contract,contract_content);
//    BOOST_CHECK(ret);*/
//
//}
/*
BOOST_AUTO_TEST_CASE(unserialize)
{
    std::string hex = "3030303032323153484300b0b846eb9eee3fbffa3193e62e86291727e8c8f15c5c397d4310883bab9653200032303138303830384d4d4d4d80841e00000000004243480ad7233c0000704148495541464549484641465745464153444645";
    Action ac;
    std::string hex_C1 = "303030303131315f2a8eda8ad4d1a35bbd7e3b22bc970e8ebe68e3f8bc4fde5c34b546543339e9e02eb3c21c704e5f497637eb3050ed6221baea375909b8bc56aff4a4241b70db6368696e6163686c617732303138303830382f2f68747470733a2f2f7777772e717569636b626c6f636b2e696e666f2f6263686c61622f746f6b656e697a6174696f6e2f636f6e74726163742f636f6e74656e742f6263686263686c6162735f746f6b656e697a654f62634d0ad7a33c4243480010a5d4e8000000";
    std::string hex_C2 = "303030303132315f2a8eda8ad4d1a35bbd7e3b22bc970e8ebe68e3f8bc4fde5c34b546543339e9e02eb3c21c704e5f497637eb3050ed6221baea375909b8bc56aff4a4241b70db6368696e6163686c6177323031383037303832303138303830382f2f68747470733a2f2f7777772e717569636b626c6f636b2e696e666f2f6263686c61622f746f6b656e697a6174696f6e2f636f6e74726163742f636f6e74656e742f62636839396263686c6162735f746f6b656e697a654f6263686c6162735f746f6b656e697a6562634d0ad7a33c4243480010a5d4e8000000";
    std::string hex_A1 = "30303030323131534843a1eec1e330e99b386853322a04f00d76355465d967c0fac36cfb0e1117bd9b044d4d4d4d10270000000000004243480ad7233c00007041";
    std::string hex_A2 = "30303030323231534843a1eec1e330e99b386853322a04f00d76355465d967c0fac36cfb0e1117bd9b04200032303138303830384d4d4d4d10270000000000004243480ad7233c00007041";
    std::string hex_T2 = "30303030333231534843ea32329206dad59fc8075f352ea683ad3428a0fe6ad84d6c98faeead7366fc1ce8030000000000003230313830323032424348cdcc4c3ecdcc4c3e71716c30366d7368667a38366d75786539307a6c74376d7263383565356d6a6e3071616e767334756366";
    std::string hex_T4 = "30303030333431534843ea32329206dad59fc8075f352ea683ad3428a0fe6ad84d6c98faeead7366fc1c18fcffffffffffffe8030000000000003230313831323132";
    // ac.UnSerializeOperation(hex_C1);
    std::cout << "------------------------------------------------"<<std::endl;
    // ac.UnSerializeOperation(hex_A1);
    std::cout << "------------------------------------------------"<<std::endl;
    // ac.UnSerializeOperation(hex_T1);
    std::cout << "short size :" << sizeof(short) << std::endl;
    std::cout << "uint16_t size :" << sizeof(uint16_t) << std::endl;
    // ac.UnSerializeOperation(hex_T4);
    ac.UnSerializeOperation(hex);
}

BOOST_AUTO_TEST_CASE(address)
{
    std::string reg_address = "bchreg:qzys8h63u04l34jw5hrcqxsenrrarkz6dg3tfer8fn";
    if(reg_address.size() == 49)
        reg_address = reg_address.substr(7);
    std::cout<<"address: "<<reg_address<<std::endl;
}

BOOST_AUTO_TEST_CASE(assettypes)
{
    Asset::Shc* shc = new Asset::Shc();
    shc->version = "1";
    shc->trading_restricion = "SHC";
    shc->dividend_type = "3";
    shc->dividend_var = 0.1;
    shc->dividend_fixed = 0.1;
    shc->disttribution_interval = "4";
    shc->guaranteed = "Y";
    shc->ticker = "NNNN";
    shc->ISIN = "MMMMMMMMMMMM";
    shc->description = "HHHHHHHHHHHHHHHHHHH";
    std::string payload_data;

    Asset asset;
    asset.Serialize((Asset::Payload*)shc,Asset::AssetType::SHC,payload_data);

    Asset::Shc* unshc = new Asset::Shc();
    asset.UnSerialize((Asset::Payload*)unshc,Asset::AssetType::SHC,payload_data);

    BOOST_CHECK_EQUAL("1",unshc->version);
    BOOST_CHECK_EQUAL("SHC",unshc->trading_restricion);
    BOOST_CHECK_EQUAL("3",unshc->dividend_type);
    BOOST_CHECK_EQUAL(0.1,unshc->dividend_var);
    BOOST_CHECK_EQUAL(0.2,unshc->dividend_fixed);
    BOOST_CHECK_EQUAL("4",unshc->disttribution_interval);
    BOOST_CHECK_EQUAL("Y",unshc->guaranteed);
    BOOST_CHECK_EQUAL("NNNN",unshc->ticker);
    BOOST_CHECK_EQUAL("MMMMMMMMMMMM",unshc->ISIN);
    BOOST_CHECK_EQUAL("HHHHHHHHHHHHHHHHHHH",unshc->description);
}
*/

BOOST_AUTO_TEST_SUITE_END()
