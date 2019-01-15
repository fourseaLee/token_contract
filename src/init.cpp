#include "init.h"
#include <glog/logging.h>
#include <fstream>
#include <sstream>
#include "common/json.hpp"
#include  <string>
#include "common/util.h"
#include <iostream>
#include "data/db_mysql.h"

std::map<std::string,std::string > g_map_conf;

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
            g_map_conf[key] = val;
        }
    }
}


bool InitConfigure()
{

    std::ifstream configure_file( g_congfigure_path);

    if (!configure_file)
    {
        std::cout << "No such " << g_congfigure_path << " config file!";
        return  false;
    }

    json json_conf;
    configure_file >> json_conf;
    if(!json_conf.is_object())
    {
        std::cout << "This " << g_congfigure_path << " is not json file" << std::endl;
        json_conf.clear();
        return  false;
    }
    ReadConfJsonObj(json_conf);
    json_conf.clear();
    configure_file.close();

    std::ifstream log_file(g_log_path);
    if (!log_file)
    {
        std::cout << "No such " << g_log_path << " log file!";
        return  false;
    }

    json json_log;
    log_file >> json_log;
    if(!json_log.is_object())
    {
        std::cout << "This " << g_log_path << " is not json file" << std::endl;
        json_log.clear();
        return  false;
    }
    ReadConfJsonObj(json_log);
    json_log.clear();
    log_file.close();
    return true;
}

bool InitLog()
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


