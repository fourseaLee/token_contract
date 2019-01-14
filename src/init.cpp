#include "init.h"
#include <glog/logging.h>
#include <fstream>
#include <sstream>
#include "json.hpp"
#include  <string>
#include "interactive/tokeninteractive.h"
#include <iostream>
#include "consumer.h"
#include "consumer_handler.h"
#include "data/db_mysql.h"

std::map<std::string,std::string > token_init::g_map_conf;
ProducerKafka* token_init::g_kafka_producer = new ProducerKafka;
std::string token_init::g_kafka_producer_topic;

static void ReadConfJsonObj(const json json_data)
{
    auto it = json_data.begin();
    auto end = json_data.end();
    for (; it !=end; ++it)
    {
        std::string key = it.key();
        auto val = it.value();
        if(val.is_object())
        {
            ReadConfJsonObj(val);
        }
        else if(val.is_string())
        {
            // mapArgs_[it.key()] = val;
            token_init::g_map_conf[key] = val;
        }
    }
}


bool token_init::InitConfigure()
{

    std::ifstream configure_file(token_interactive::g_congfigure_path);

    if (!configure_file)
    {
        std::cout << "No such " << token_interactive::g_congfigure_path << " config file!";
        return  false;
    }

    json json_conf;
    configure_file >> json_conf;
    if(!json_conf.is_object())
    {
        std::cout << "This " << token_interactive::g_congfigure_path << " is not json file" << std::endl;
        json_conf.clear();
        return  false;
    }
    ReadConfJsonObj(json_conf);
    json_conf.clear();
    configure_file.close();

    std::ifstream log_file(token_interactive::g_log_path);
    if (!log_file)
    {
        std::cout << "No such " << token_interactive::g_log_path << " log file!";
        return  false;
    }

    json json_log;
    log_file >> json_log;
    if(!json_log.is_object())
    {
        std::cout << "This " << token_interactive::g_log_path << " is not json file" << std::endl;
        json_log.clear();
        return  false;
    }
    ReadConfJsonObj(json_log);
    json_log.clear();
    log_file.close();
    return true;
}

bool token_init::InitLog()
{
    std::string log_path  = g_map_conf["log_path"];
    FLAGS_alsologtostderr = false;
    FLAGS_colorlogtostderr = true;
    FLAGS_max_log_size = 100;
    FLAGS_stop_logging_if_full_disk  = true;
    std::string log_exec = "log_exe";
    google::InitGoogleLogging(log_exec.c_str());
    FLAGS_log_dir = g_map_conf["log_path"];
    std::string log_dest = log_path+"/info_";
    google::SetLogDestination(google::GLOG_INFO,log_dest.c_str());
   // log_dest = log_path+"/warn_";
  //  google::SetLogDestination(google::GLOG_WARNING,log_dest.c_str());
    log_dest = log_path+"/error_";
  //  google::SetLogDestination(google::GLOG_ERROR,log_dest.c_str());
    log_dest = log_path+"/fatal_";
 //   google::SetLogDestination(google::GLOG_FATAL,log_dest.c_str());
    google::SetStderrLogging(google::GLOG_ERROR);

    return true;
}

bool token_init::InitKafkaProducer()
{

    std::string kafka_url = g_map_conf["kafka_url"];// g_conf.getArgs("kafka_url","47.99.82.55:9092");
    g_kafka_producer_topic = g_map_conf["producer"];// g_conf.getArgs("producer","notetxs");

    if (PRODUCER_INIT_SUCCESS != g_kafka_producer->InitKafka(0, kafka_url.c_str(), g_kafka_producer_topic.c_str()))
    {
       return false;
    }

    return true;

}

bool token_init::InitKafkaConsumer()
{
    std::string topics  = g_map_conf["topic"];//  g_map_conf.getArgs("consumer_topic", "notetxs1");
    std::string brokers = g_map_conf["brokers"];//g_map_conf.getArgs("brokers", "47.99.82.55:9092");
    std::string group = g_map_conf["groups"];//g_map_conf.getArgs("groups", "1");

    std::string select_sql = "select offset from kafka_info where topic = '" + topics +"';";


    DBMysql::JsonDataFormat json_format;
    json_format.column_size = 1;
    json_format.map_column_type[0] = DBMysql::INT;

    json json_data;
    bool ret = g_db_mysql->GetDataAsJson(select_sql, &json_format, json_data);

    if (!ret)
    {
        LOG(ERROR) << "init coin type from db fail.";
        return false;
    }

    int offset = 0;
    if (!json_data.is_null())
    {
        offset = json_data[0][0].get<int>();;
    }
    else
    {
        std::string sql_insert  = "INSERT INTO `kafka_info` (`topic`, `offset`) VALUES ('" + topics +"', '0');";
        g_db_mysql->QuerySql(sql_insert);
    }

    std::cout << "offset: " << offset << std::endl;
    std::shared_ptr<CConsumer> consumer = std::make_shared<CConsumer>(brokers, topics, group, offset);

    ConsumerHandler* handler = new ConsumerHandler();

    handler->SetTopic(g_map_conf["topic"]);
    if (!consumer->Init())
    {
        std::cout << "init kafka consumer failed " << std::endl;
        return false;
    }
    else
    {
        std::cout << "start kafka consumer.\n";
        consumer->SetHandler(handler);
        consumer->Consume(1000);
    }
    return true;
}


