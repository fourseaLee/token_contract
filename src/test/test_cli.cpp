#include "testingsetup.h"
#include "transaction/txobject.h"
#include "interactive/tokeninteractive.h"
#include "contract/contractobject.h"
#include "contract_machine/contractparse.h"
#include "logiccli.h"
#include  "data/db_mysql.h"
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

BOOST_AUTO_TEST_SUITE(cli)

void OutputFun(const std::string& func,const std::string& result)
{
	std::cout << func << " : " << std::endl;
	std::cout << "result: " << std::endl;
	std::cout << result << std::endl; 
}
BOOST_AUTO_TEST_CASE(format)
{
    
	json json_params;
    json_params["issuer_address"] = "qzys8h63u04l34jw5hrcqxsenrrarkz6dg3tfer8fn";
    json_params["contract_address"] = "qzys8h63u04l34jw5hrcqxsenrrarkz6dg3tfer8fn";
    json_params["contract_name"] = "test";
    json_params["contract_content"] ="content";
    json_params["contract_fee"] = 0.0005;
    json_params["miner_fee"] = 0.000005;
    json_params["priv_key_base58"] = "qzys8h63u04l34jw5hrcqxsenrrarkz6dg3tfer8fn";
    json json_result = json::array();
    json json_utxo = json::array();
    json_utxo.push_back("a3115b3d0b048cf65b5a1d9b08ad806cfe8a9c818bbefd300924907de0a6b5fd");
    json_utxo.push_back(1);
    json_utxo.push_back("0.0005");
    json_result.push_back(json_utxo);
    json_utxo.clear();
    json_utxo.push_back("f15502fefb5f96ebc6b2c0d413e278a0ea9d047ad381f109eb7b207b49bc49fb");
    json_utxo.push_back(1);
    json_utxo.push_back("0.00041");
    json_result.push_back(json_utxo);
    json_params["utxo"] = json_result;
    std::ofstream stream_file("contract_offer.json");
    stream_file  << json_params.dump() ;
    stream_file.close();
	json json_output;
	std::ifstream stream_out("contract_offer.json");
	stream_out >> json_output ;
	stream_out.close();
	std::cout <<  "output: " << json_output.dump() << std::endl;

	json_params.clear();
	json_params["issuer_address"] = "qzys8h63u04l34jw5hrcqxsenrrarkz6dg3tfer8fn";
    json_params["contract_address"] = "qzys8h63u04l34jw5hrcqxsenrrarkz6dg3tfer8fn";
	json_params["asset_id"] = "SHC";
	json_params["qty"] = 1000000000;
	json_params["contract_fee"] = 0.00005000;
	json_params["miner_fee"] = 0.00001000;
	json_params["priv_key_base58"] = "qzys8h63u04l34jw5hrcqxsenrrarkz6dg3tfer8fn";
	json_params["utxo"] = json_result;
	stream_file.open("asset_definition.json");
    stream_file  << json_params.dump() ;
    stream_file.close();
	json_output.clear();
	stream_out.open("asset_definition.json");
	stream_out >> json_output ;
	stream_out.close();
	std::cout <<  "asset definition: " << json_output.dump() << std::endl;

	json_params.clear();
	json_params["contract_address"] = "qzys8h63u04l34jw5hrcqxsenrrarkz6dg3tfer8fn";
	json_params["currency"] = 5.0;
	json_params["contract_fee"] = 0.00005000;
	json_params["miner_fee"] = 0.00001000;
	json_params["utxo_send"] = json_result;
	json_params["utxo_recieve"] = json_result;
	json_params["send_address"] = "qqdyqu0xn3qkmwwu34t9fxrg6zwqmms4t5kgsyneex";
	json_params["recieve_address"] = "qr9mvay6fuey8y8c0k7qsykgm8aq05jygv4d89jyzz";
	json_params["asset_type"] = "SHC";
	json_params["asset_id"] =  "test";
	json_params["shares"] = 100000;
	json_params["priv_key_base58"] = "qzys8h63u04l34jw5hrcqxsenrrarkz6dg3tfer8fn";
	stream_file.open("token_send.json");
    stream_file  << json_params.dump() ;
    stream_file.close();
	json_output.clear();
	stream_out.open("token_send.json");
	stream_out >> json_output ;
	stream_out.close();
	std::cout <<  "token send: " << json_output.dump() << std::endl;

	json_params.clear();
	json_params["priv_key_base58"] = "cVyhQhEfRJiPhcRS9AMP1jkzokmQG719BeFUnd9fmNSE1BPcB5WH";
	json_params["tx_hex"] = "0200000004fdb5a6e07d90240930fdbe8b819c8afe6c80ad089b1d5a5bf68c040b3d5b11a30100000000fffffffffb49bc497b207beb09f181d37a049deaa078e213d4c0b2c6eb965ffbfe0255f10100000000fffffffffdb5a6e07d90240930fdbe8b819c8afe6c80ad089b1d5a5bf68c040b3d5b11a30100000000fffffffffb49bc497b207beb09f181d37a049deaa078e213d4c0b2c6eb965ffbfe0255f10100000000ffffffff04905f0100000000001976a914000000000000000000000000000000000000000088acf04f0100000000001976a914000000000000000000000000000000000000000088ac8878cd1d000000001976a914000000000000000000000000000000000000000088ac00000000000000000000000000";
	json_params["utxo"] = json_result;
	stream_file.open("token_recieve.json");
    stream_file  << json_params.dump() ;
    stream_file.close();
	json_output.clear();
	stream_out.open("token_recieve.json");
	stream_out >> json_output ;
	stream_out.close();
	std::cout <<  "token recieve: " << json_output.dump() << std::endl;

}

BOOST_AUTO_TEST_CASE(GetUtxo)
{
	LogicCli logic;
	std::string contract_url = "http://192.168.0.26:6666";
	logic.SetContractUrl(contract_url);	
	std::string fun = "GetUtxo";
	json json_params ;
	json_params["address"] = "qzys8h63u04l34jw5hrcqxsenrrarkz6dg3tfer8fn";
	json_params["value"] = "0.00005000";
	std::string result;
	logic.RunFunction(fun,json_params.dump(),result);
	OutputFun(fun,result);
	
}

BOOST_AUTO_TEST_CASE(ContractOffer)
{
	LogicCli logic;
	std::string contract_url = "http://192.168.0.26:6666";
	logic.SetContractUrl(contract_url);	
	std::string fun = "ContractOffer";
	json json_params ;
	std::ifstream stream_in("contract_offer.json");
	stream_in >> json_params ;
	stream_in.close();
	std::string result;
	logic.RunFunction(fun,json_params.dump(),result);
	OutputFun(fun,result);
	
}

BOOST_AUTO_TEST_CASE(AssetDefinition)
{
	LogicCli logic;
	std::string contract_url = "http://192.168.0.26:6666";
	logic.SetContractUrl(contract_url);	
	std::string fun = "AssetDefinition";
	json json_params ;
	std::ifstream stream_in("asset_definition.json");
	stream_in >> json_params ;
	stream_in.close();
	std::string result;
	logic.RunFunction(fun,json_params.dump(),result);
	OutputFun(fun,result);
}

BOOST_AUTO_TEST_CASE(TokenSend)
{
	LogicCli logic;
	std::string contract_url = "http://192.168.0.26:6666";
	logic.SetContractUrl(contract_url);	
	std::string fun = "TokenSend";
	json json_params ;
	std::ifstream stream_in("token_send.json");
	stream_in >> json_params ;
	stream_in.close();
	std::string result;
	logic.RunFunction(fun,json_params.dump(),result);
	OutputFun(fun,result);
}

BOOST_AUTO_TEST_CASE(TokenRecieve)
{
	std::cout << "test recieve: "<< std::endl;
	LogicCli logic;
	std::string contract_url = "http://192.168.0.26:6666";
	logic.SetContractUrl(contract_url);	
	std::string fun = "TokenRecieve";
	json json_params ;
	std::ifstream stream_in("token_recieve.json");
	stream_in >> json_params ;
	stream_in.close();
	std::string result;
	logic.RunFunction(fun,json_params.dump(),result);
	OutputFun(fun,result);
}

BOOST_AUTO_TEST_CASE(standard)
{
	LogicCli logic;
	std::string contract_url = "http://192.168.0.40:18332";
	logic.SetContractUrl(contract_url);

	std::string cli_function;
	std::string cli_params;
	std::string result;
	cli_function = "Help";
	logic.RunFunction(cli_function,cli_params,result);
	OutputFun(cli_function,result);
	cli_function = "GetUtxo";
	json json_params;
	json_params["address"] = "qzys8h63u04l34jw5hrcqxsenrrarkz6dg3tfer8fn";
	json_params["value"] = "0.9";
	cli_params = json_params.dump();
	//logic.RunFunction(cli_function,cli_params,result);
	OutputFun(cli_function,result);
    json_params.clear();

    cli_function = "ContractOffer";
    json_params["issuer_address"] = "qzys8h63u04l34jw5hrcqxsenrrarkz6dg3tfer8fn";
    json_params["contract_address"] = "qzys8h63u04l34jw5hrcqxsenrrarkz6dg3tfer8fn";
    json_params["contract_name"] = "test";
    json_params["contract_content"] ="content";
    json_params["contract_fee"] = 0.0005;
    json_params["miner_fee"] = 0.000005;
    json_params["priv_key_base58"] = "qzys8h63u04l34jw5hrcqxsenrrarkz6dg3tfer8fn";
    json json_result = json::array();
    json json_utxo = json::array();
    json_utxo.push_back("a3115b3d0b048cf65b5a1d9b08ad806cfe8a9c818bbefd300924907de0a6b5fd");
    json_utxo.push_back(1);
    json_utxo.push_back("0.0005");
    json_result.push_back(json_utxo);
    json_utxo.clear();
    json_utxo.push_back("f15502fefb5f96ebc6b2c0d413e278a0ea9d047ad381f109eb7b207b49bc49fb");
    json_utxo.push_back(1);
    json_utxo.push_back("0.00041");
    json_result.push_back(json_utxo);
    json_params["utxo"] = json_result;
    std::ofstream stream_file("contract_offer.json");
    stream_file  << json_params.dump() ;
    stream_file.close();
}
BOOST_AUTO_TEST_SUITE_END()

