#ifndef INIT_H
#define INIT_H
#include <map>
#include<string>
#include "producer.h"
#include <mutex>

namespace token_init
{
    bool InitConfigure();
    bool InitLog();
    bool InitKafkaProducer();
    bool InitKafkaConsumer();

    extern std::map<std::string,std::string> g_map_conf;
    extern  ProducerKafka* g_kafka_producer;
    extern std::string g_kafka_producer_topic;

}

#endif // INIT_H
