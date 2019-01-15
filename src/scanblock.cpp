#include <iostream>
#include "init.h"
#include "common/util.h"
#include <thread>
#include "parsetx.h"
#include "data/db_mysql.h"
#include "httpserver.h"

static void  ScanBlockChainData()
{
    ParseOpreturnData parse_opreturn_data(g_map_conf["block_node_url"],g_map_conf["rpc_auth"]);
    DBMysql::MysqlConnect* connect = new DBMysql::MysqlConnect();
    connect->use_db =  g_map_conf["mysqldb"];
    connect->user_pass =  g_map_conf["mysqlpass"];
    connect->port = std::atoi(g_map_conf["mysqlport"].c_str());
    connect->user_name = g_map_conf["mysqluser"];
    connect->url = g_map_conf["mysqlserver"];
    int pre_block_height =0;
    std::string select_sql = "select height from  txinfo  order by  height desc limit 1;";
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

    select_sql = "select txid from  txinfo where height = -1;";
    DBMysql::JsonDataFormat json_format_txid;
    json_format_txid.column_size = 1;
    json_format_txid.map_column_type[0] = DBMysql::STRING;
    json json_data_txid;
    g_db_mysql->GetDataAsJson(select_sql,&json_format_txid,json_data_txid);
    std::vector<std::string> vect_mempool_tx;
    for (unsigned int i = 0; i < json_data_txid.size(); i++ )
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
        parse_opreturn_data.FlushToDB();

        parse_opreturn_data.SetFilterType(ParseTx::INMEMPOOL);
        parse_opreturn_data.FilterBlock();
        parse_opreturn_data.FlushToDB();

        parse_opreturn_data.FlushUtxoToDB();
        
        sleep(1);
    }
}

static void PrintAllConf()
{
    std::map<std::string,std::string>::const_iterator con_it =  g_map_conf.begin();
    while(con_it != g_map_conf.end())
    {
        std::cout << con_it->first << ":" << con_it->second;
        std::cout << std::endl;
        con_it++;
    }
}

int main(int argc, char* argv[])
{
    std::cout << "just for test" <<std::endl;

    if ( !ParseCmd(argc,argv) )
    {
        std::cout <<  "must be init the configure path and log path " << std::endl;
        return 0;
    }

    if ( !InitConfigure() )
    {
        std::cout << "init the configure  failed!" << std::endl;
        return 0;
    }
    PrintAllConf();

    if ( !InitLog() )
    {
        std::cout << "init the log failed!" << std::endl;
        return 0;
    }
    ScanBlockChainData();
    return 0;
}
