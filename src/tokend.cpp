#include <iostream>
#include "init.h"
#include "interactive/tokeninteractive.h"
#include <thread>
#include "parsetx.h"
#include "data/db_mysql.h"
#include "httpserver.h"

//this function move to "scanblock.cpp" where is  another executable model
/***
void  ScanBlockChainData()
{
    ParseOpreturnData parse_opreturn_data(token_init::g_map_conf["block_node_url"],token_init::g_map_conf["rpc_auth"]);
    DBMysql::MysqlConnect* connect = new DBMysql::MysqlConnect();
    connect->use_db =  token_init::g_map_conf["mysqldb"];
    connect->user_pass =  token_init::g_map_conf["mysqlpass"];
    connect->port = std::atoi(token_init::g_map_conf["mysqlport"].c_str());
    connect->user_name = token_init::g_map_conf["mysqluser"];
    connect->url = token_init::g_map_conf["mysqlserver"];

    int pre_block_height =0;
    std::string select_sql = "select hight from  txinfo  order by  hight desc limit 1;";
    DBMysql::JsonDataFormat json_format;
    json_format.column_size = 1;
    json_format.map_column_type[0] = DBMysql::INT;

    g_db_mysql->SetConnect(connect);
    if (!g_db_mysql->OpenDB())
    {
        return;
    }
    json json_data;
    g_db_mysql->GetDataAsJson(select_sql,&json_format,json_data);
    if (json_data.size() == 1)
    {
        pre_block_height = json_data[0][0].get<int>();
    }

    select_sql = "select txid from  txinfo where hight = -1;";
    DBMysql::JsonDataFormat json_format_txid;
    json_format_txid.column_size = 1;
    json_format_txid.map_column_type[0] = DBMysql::STRING;
    json json_data_txid;
    g_db_mysql->GetDataAsJson(select_sql,&json_format_txid,json_data_txid);
    std::vector<std::string> vect_mempool_tx;
    for (int i = 0; i < json_data_txid.size(); i++ )
    {
        json json_row = json_data_txid.at(i);
        vect_mempool_tx.push_back(json_row.at(0).get<std::string>());
    }

    parse_opreturn_data.InitMempoolTx(vect_mempool_tx);


    parse_opreturn_data.SetPreBlockHeight(pre_block_height);
    while (true)
    {
        parse_opreturn_data.SetFilterType(ParseTx::ONCHAIN);
        parse_opreturn_data.FilterBlock();
       // parse_opreturn_data.FlashToDB();

        parse_opreturn_data.SetFilterType(ParseTx::INMEMPOOL);
        parse_opreturn_data.FilterBlock();
      //  parse_opreturn_data.FlashToDB();
    }
}
**/

static void HandleKafka()
{
    if(!token_init::InitKafkaConsumer())
    {
        std::cout << "init kafka consumer" << std::endl;
    }
}



bool  InitDb()
{
    DBMysql::MysqlConnect* connect = new DBMysql::MysqlConnect();
    connect->use_db =  token_init::g_map_conf["mysqldb"];
    connect->user_pass =  token_init::g_map_conf["mysqlpass"];
    connect->port = std::atoi(token_init::g_map_conf["mysqlport"].c_str());
    connect->user_name = token_init::g_map_conf["mysqluser"];
    connect->url = token_init::g_map_conf["mysqlserver"];


    g_db_mysql->SetConnect(connect);
    if (!g_db_mysql->OpenDB())
    {
        return false;
    }

    return true;

}



bool InitHttpServer()
{
    signal(SIGHUP, SignalHandler);
    signal(SIGTERM, SignalHandler);
    signal(SIGINT, SignalHandler);
   // signal(SIGQUIT, SignalHandler);


    std::string httpd_option_listen = token_init::g_map_conf["bindaddr"];
    int httpd_option_port = std::stoi(token_init::g_map_conf["listenport"].c_str());
    int httpd_option_timeout = std::stoi(token_init::g_map_conf["timeout"].c_str());

    event_init();
    struct evhttp *httpd;


    RegisterHTTPHandler("/issueToken",issueToken);
    RegisterHTTPHandler("/transferToken",transferToken);
    RegisterHTTPHandler("/queryToken",queryToken);

    RegisterHTTPHandler("/ContractOffer",ContractOffer);
    RegisterHTTPHandler("/AssetDefinition",AssetDefinition);
    RegisterHTTPHandler("/PushUtxo",PushUtxo);

    RegisterHTTPHandler("/GetUtxo",GetUtxo);
    RegisterHTTPHandler("/SendTransaction",SendTransaction);

    httpd = evhttp_start(httpd_option_listen.c_str(), httpd_option_port);
    if(!httpd)
    {
        return false;
    }

    evhttp_set_allowed_methods(httpd, EVHTTP_REQ_POST);
    evhttp_set_timeout(httpd, httpd_option_timeout);
    evhttp_set_gencb(httpd, HttpRequestCb, nullptr);

    event_dispatch();
    evhttp_free(httpd);
    return true;

}

static void PrintAllConf()
{
    std::map<std::string,std::string>::const_iterator con_it =  token_init::g_map_conf.begin();
    while(con_it != token_init::g_map_conf.end())
    {
        std::cout << con_it->first << ":" << con_it->second;
        std::cout << std::endl;
        con_it++;
    }

}

int main(int argc, char* argv[])
{

    if ( !token_interactive::ParseCmd(argc,argv) )
    {
        std::cout <<  "must be init the configure path and log path " << std::endl;
        return 0;
    }

    if ( !token_init::InitConfigure() )
    {
        std::cout << "init the configure  failed!" << std::endl;
        return 0;
    }

    if ( !token_init::InitLog())
    {
        std::cout << "init the log failed!" << std::endl;
        return 0;
    }

    PrintAllConf();

    if ( !InitDb())
    {
        std::cout << "Init db failed!" << std::endl;
        return 0;
    }

    if( !token_init::InitKafkaProducer())
    {
        std::cout << "init kafka failed !" << std::endl;
        return 0;
    }

    std::thread th(HandleKafka);
    th.detach();


    if( !InitHttpServer())
    {
        std::cout << "init http server fail ,please check configure" << std::endl;
        return 0;
    }

    return 0;
}
