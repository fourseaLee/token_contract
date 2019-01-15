#include "init.h"
#include <iostream>
#include <glog/logging.h>
#include "common/util.h"
#include "contract/contractobject.h"
#include "contract/designcontract.h"
#include "internal/handlercli.h"
#include <boost/program_options.hpp>
#include "data/db_mysql.h"


static bool ReadParamsFromFile(const std::string& params_file,std::string&param_value)
{
    std::ifstream stream_file(params_file);

    if (!stream_file)
    {
        std::cout << "No such " << params_file << " config file!";
        return  false;
    }

    json json_conf;
    stream_file >> json_conf;
    if(!json_conf.is_object())
    {
        std::cout << "This " << params_file << " is not json file" << std::endl;
        json_conf.clear();
        return  false;
    }
    param_value = json_conf.dump();
    json_conf.clear();
    stream_file.close();
    return true;
}

static bool ParseCliParams(int argc, char*argv[])
{
    using namespace  boost::program_options;
    std::string cli_function;
    std::string cli_params;
    std::string contract_url;
    std::string params_file;

    boost::program_options::options_description opts_desc("All options");
    opts_desc.add_options()
            ("help,h","help info")
            ("function,f",value<std::string>(&cli_function)->default_value("help"),"show cli help")
            ("params,p",value<std::string>(&cli_params)->default_value("{}"),"show client params")
            ("json,j",value<std::string>(&params_file)->default_value("./param.json"),"show a json file")
            ("url,u",value<std::string>(&contract_url)->default_value("http://192.168.0.40:8888"),"contract url");

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

    HandlerCli handler;
    handler.SetContractUrl(contract_url);

    if (cmd_param_map.count("function"))
    {
        cli_function = cmd_param_map["function"].as<std::string>();
    }

    if (cmd_param_map.count("params"))
    {
        cli_params = cmd_param_map["params"].as<std::string>();
    }

    if(cmd_param_map.count("json"))
    {
        params_file = cmd_param_map["json"].as<std::string>();
        ReadParamsFromFile(params_file,cli_params);
    }

    std::string result;
    handler.RunFunction(cli_function,cli_params,result);
    std::cout << "result :" << std::endl;
    std::cout << result << std::endl;
    return true;
}

int main(int argc, char *argv[])
{
    ParseCliParams(argc,argv);
    return 0;
}
