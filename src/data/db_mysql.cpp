#include "db_mysql.h"
#include <glog/logging.h>
#include <utility>

DBMysql* g_db_mysql = new DBMysql();

DBMysql::DBMysql()
{
	connect_ = nullptr;
}

DBMysql::~DBMysql()
{
	delete connect_;
}


bool DBMysql::OpenDB()
{
	if (!connect_)
		return false;

	if (mysql_init(&mysql_) == NULL) {
		//std::cout << "初始化数据库失败" << std::endl;
		return false;
	}


	if (!mysql_real_connect(&mysql_, connect_->url.c_str(), connect_->user_name.c_str(),
							connect_->user_pass.c_str(),connect_->use_db.c_str(), 
							connect_->port, NULL, 0))
	{
		std::string error= mysql_error(&mysql_);
		LOG(ERROR) << "openDB : " << "数据库连接失败:"<<error;
		return false;
	}

	return true;
}

void DBMysql::SetConnect(MysqlConnect*connect)
{
	connect_ = connect;
}

void DBMysql::GetConnect(MysqlConnect*connect)
{
    connect = connect_;
}

void DBMysql::QuerySql(const std::__cxx11::string &sql_query)
{
    mtx_.lock();
    int ret = mysql_real_query(&mysql_, sql_query.c_str(), strlen(sql_query.c_str()));
    if (ret != 0 && mysql_errno(&mysql_) != 1062)
    {
        LOG(ERROR) << "exec sql failed" << sql_query ;
    }
    mtx_.unlock();
}

void DBMysql::BatchQuerySql(const std::vector<std::string> &vect_sql_query)
{
    mysql_query(&mysql_,"START TRANSACTION");
    for(uint32_t i = 0; i < vect_sql_query.size(); i ++)
    {
        QuerySql(vect_sql_query.at(i));
    }
    mysql_query(&mysql_,"COMMIT");
}


bool DBMysql::GetDataAsJson(const std::string& select_sql, JsonDataFormat* json_data_format ,json& json_data)
{
    if (json_data_format->column_size != json_data_format->map_column_type.size())
    {
        LOG(ERROR) << "json data format size error!";
        return false;
    }


    int ret = mysql_real_query(&mysql_,select_sql.c_str(), strlen(select_sql.c_str()));
    MYSQL_RES *result = mysql_store_result(&mysql_);
    size_t num_rows = mysql_num_rows(result);

    for (size_t i = 0; i < num_rows; ++i)
    {
        MYSQL_ROW row = mysql_fetch_row(result);
        json row_data = json::array();

        for (size_t j = 0; j < json_data_format->column_size; j++)
        {
            std::string sql_data = row[j];
            JsonDataType type  = json_data_format->map_column_type[j];
            if ( INT == type )
            {
                int real_data = std::stoi(sql_data);
                row_data.push_back(real_data);
            }
            else if ( DOUBLE == type )
            {
                double real_data = std::stof(sql_data);
                row_data.push_back(real_data);
            }
            else if ( STRING == type )
            {
                row_data.push_back(sql_data);
            }
        }
        json_data.push_back(row_data);
    }
    mysql_free_result(result);

    return true;
}


void DBMysql::CloseDB()
{
	mysql_close(&mysql_);
}
