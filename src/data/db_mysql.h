#ifndef DB_MYSQL_H
#define DB_MYSQL_H
#include <string>
#include <mysql/mysql.h>
#include <vector>
#include "common/json.hpp"
#include <mutex>
using json = nlohmann::json;
class DBMysql
{
public:
	DBMysql();
	~DBMysql();

	struct MysqlConnect
	{
		std::string url;
		int			port;
		std::string user_name;
		std::string user_pass;
		std::string use_db;
	};

    enum JsonDataType
    {
        INT = 1,
        DOUBLE = 2,
        STRING =3
    };

    struct JsonDataFormat
    {
        uint32_t column_size;
        std::map<uint32_t, JsonDataType> map_column_type;

    };

public:	
 	bool OpenDB();

	void CloseDB();

	void SetConnect(MysqlConnect*connect);

	void GetConnect(MysqlConnect*connect);
public:

    void QuerySql(const std::string&sql_query);

    void BatchQuerySql(const std::vector<std::string>& vect_sql_query);
    
    bool GetDataAsJson(const std::string& select_sql, JsonDataFormat* json_data_format ,json& json_data);
    

private:
	MYSQL mysql_;
	MysqlConnect* connect_;
    std::mutex mtx_;
};

extern DBMysql* g_db_mysql;

#endif
