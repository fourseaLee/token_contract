#include "testingsetup.h"
#include "transaction/txobject.h"
#include "interactive/tokeninteractive.h"
#include "contract/contractobject.h"
#include "contract_machine/contractparse.h"
#include <iostream>
#include <string>

BOOST_AUTO_TEST_SUITE(interactive)
BOOST_AUTO_TEST_CASE(standard)
{
/*    ContractObject contract_obj;
    std::string priv_key;
    std::string pub_key;
    ContractObject::GenerateEncryptKey(priv_key,pub_key);
    contract_obj.SetPrivateKey(priv_key);
    std::cout << "private key : " << priv_key << std::endl;
    std::cout << "public key :" << pub_key  << std::endl;

    std::cout << std::endl << std::endl << std::endl;
   // std::cout << "5df6e0e2761359d3a827558e299fcc381534545f55cf43e41983f5d4c9456" << std::endl;
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
    BOOST_CHECK(ret);*/

}
BOOST_AUTO_TEST_SUITE_END()

