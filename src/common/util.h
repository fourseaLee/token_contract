#ifndef TOKENINTERACTIVE_H
#define TOKENINTERACTIVE_H
#include <string>

struct CurlParams
{
    std::string curl_url;
    std::string curl_post_data;
    std::string curl_auth;
    bool need_auth;
    std::string curl_response;
    CurlParams()
    {
        need_auth = false;
    }
};
bool ParseCmd(int argc,char* argv[]);
bool CurlPost(CurlParams* curl_params);
extern std::string g_log_path;
extern std::string g_congfigure_path;

#endif // TOKENINTERACTIVE_H
