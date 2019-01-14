#include "consumer_handler.h"
#include "data/db_mysql.h"
#include "json.hpp"
#include <iostream>
#include <glog/logging.h>
#include "logickafka.h"

using json = nlohmann::json;

ConsumerHandler::ConsumerHandler()
{

}


ConsumerHandler::~ConsumerHandler()
{

}

void ConsumerHandler::UpdateDB(int offset)
{
    std::string update_sql = "UPDATE kafka_info SET offset = '" + std::to_string(offset) + "' WHERE  `topic`='" + topic_ +"';";
    LOG(INFO) << update_sql ;
    g_db_mysql->QuerySql(update_sql);
}

void ConsumerHandler::HandleMsg(std::string msg, int offset)
{
    UpdateDB(offset);
    json json_msg = json::parse(msg);
    int code = json_msg["code"].get<int>();
    std::string target_address = json_msg["target"].get<std::string>();

    LogicKafka logic;
    if ( !logic.IsMyAddress(target_address))
    {
        LOG(INFO) << msg ;
        return ;
    }
    json json_contract = json_msg["content"];

    if(code == Logic::UTXO)
    {
       logic.ExchangeSign(json_contract);
    }
    else if (code == Logic::UNSIGN_TX)
    {
        std::string txid;
        logic.PushSign(json_contract,txid);
    }

}

void ConsumerHandler::SetTopic(const std::string &topic)
{
    topic_ = topic;
}




