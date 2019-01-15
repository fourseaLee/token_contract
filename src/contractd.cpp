#include <iostream>
#include "init.h"
#include "common/util.h"
#include <thread>
#include "parsetx.h"
#include "data/db_mysql.h"
#include "httpserver.h"


bool  InitDb()
{
    DBMysql::MysqlConnect* connect = new DBMysql::MysqlConnect();
    connect->use_db =  g_map_conf["mysqldb"];
    connect->user_pass =  g_map_conf["mysqlpass"];
    connect->port = std::atoi(g_map_conf["mysqlport"].c_str());
    connect->user_name = g_map_conf["mysqluser"];
    connect->url = g_map_conf["mysqlserver"];
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


    std::string httpd_option_listen = g_map_conf["bindaddr"];
    int httpd_option_port = std::stoi(g_map_conf["listenport"].c_str());
    int httpd_option_timeout = std::stoi(g_map_conf["timeout"].c_str());

    event_init();
    struct evhttp *httpd;
    RegisterHTTPHandler("/GetUtxo",GetUtxo);
    RegisterHTTPHandler("/SendTransaction",SendTransaction);
    RegisterHTTPHandler("/QueryBalance",QueryBalance);
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

    if ( !InitLog())
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

    if( !InitHttpServer())
    {
        std::cout << "init http server fail ,please check configure" << std::endl;
        return 0;
    }

    return 0;
}
