#ifndef INIT_H
#define INIT_H
#include <map>
#include<string>

bool InitConfigure();
bool InitLog();
extern std::map<std::string,std::string> g_map_conf;
#endif // INIT_H
