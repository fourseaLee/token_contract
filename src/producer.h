#ifndef  PRODUCER_H
#define  PRODUCER_H
#include <ctype.h>  
#include <signal.h>  
#include <string.h>  
#include <unistd.h>  
#include <stdlib.h>  
#include <syslog.h>  
#include <sys/time.h>  
#include <errno.h>  
#include <librdkafka/rdkafka.h>  
#include <string>
const int PRODUCER_INIT_FAILED = -1;  
const int PRODUCER_INIT_SUCCESS = 0;  
const int PUSH_DATA_FAILED = -1;  
const int PUSH_DATA_SUCCESS = 0;  

/*
static void logger(const rd_kafka_t *rk, int level,const char *fac, const char *buf)   
{  
    struct timeval tv;  
    gettimeofday(&tv, NULL);  
    fprintf(stderr, "%u.%03u RDKAFKA-%i-%s: %s: %s\n",  
            (int)tv.tv_sec, (int)(tv.tv_usec / 1000),  
            level, fac, rk ? rd_kafka_name(rk) : NULL, buf);  
} */ 


class ProducerKafka  
{  
public:  
    ProducerKafka(){}
    ~ProducerKafka(){}  

    int InitKafka(int partition, const char *brokers, const char *topic);

    void Destroy();
    int PushData(const std::string kafka_data)
    {
        return PushData(kafka_data.c_str() ,kafka_data.size());
    }
protected:
     int PushData(const char* buf, int buf_len);
private:  
    int partition_;   

    //rd  
    rd_kafka_t* handler_;  
    rd_kafka_conf_t *conf_;  

    //topic  
    rd_kafka_topic_t *topic_;  
    rd_kafka_topic_conf_t *topic_conf_;  
};  



#endif

