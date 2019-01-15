#include "common/util.h"
#include <boost/program_options.hpp>
#include <iostream>
#include <curl/curl.h>
#include <glog/logging.h>

std::string g_log_path;
std::string g_congfigure_path;
bool ParseCmd(int argc, char *argv[])
{
    using namespace  boost::program_options;
    std::string path_configure ;
    std::string path_log;

    boost::program_options::options_description opts_desc("All options");
    opts_desc.add_options()
            ("help,h","help info")
            ("log_path,l",value<std::string>(&path_log)->default_value("../conf/log.json"),"log configure ")
            ("configure_path,c",value<std::string>(&path_configure)->default_value("../conf/conf.json"),"configure path configure ");

    variables_map  cmd_param_map;
    try
    {
        store(parse_command_line(argc,argv,opts_desc),cmd_param_map);
    }
    catch(boost::program_options::error_with_no_option_name &ex)
    {
        std::cout<<ex.what()<<std::endl;
    }
    notify(cmd_param_map);

    if (cmd_param_map.count("help"))
    {
        std::cout<<opts_desc<<std::endl;
        return false;
    }
    if (cmd_param_map.count("log_path"))
    {
        path_log = cmd_param_map["log_path"].as<std::string>();
        g_log_path = path_log;
    }

    if (cmd_param_map.count("configure_path"))
    {
        path_configure = cmd_param_map["configure_path"].as<std::string>();
        g_congfigure_path = path_configure;
    }

    return true;
}

//the callback function for CurlPost
static size_t ReplyCallback(void *ptr, size_t size, size_t nmemb, void *stream)
{
    std::string *str = (std::string*)stream;
    (*str).append((char*)ptr, size*nmemb);
    return size * nmemb;
}

bool CurlPost(CurlParams *curl_params)
{
    CURL *curl = curl_easy_init();
    struct curl_slist *headers = NULL;
    CURLcode res;
    curl_params->curl_response.clear();

    std::string error_str ;
    //error_str.clear();
    if (curl)
    {
        headers = curl_slist_append(headers, "content-type:application/json;");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_URL, curl_params->curl_url.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)curl_params->curl_post_data.size());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, curl_params->curl_post_data.c_str());

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, ReplyCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&curl_params->curl_response);
        if(curl_params->need_auth)
        {
            curl_easy_setopt(curl, CURLOPT_USERPWD, curl_params->curl_auth.c_str());
        }
        curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_TRY);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 30);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30);
        res = curl_easy_perform(curl);
    }
    curl_easy_cleanup(curl);

    if (res != CURLE_OK)
    {
        error_str = curl_easy_strerror(res);
        LOG(ERROR) << error_str ;
        return false;
    }
    return true;
}
