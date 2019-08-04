#pragma once
#include "mysql.h"
#include "Util.hpp"
#define DB_ETC "./db.etc"

class MysqlClient
{
private:
  MYSQL *con;
  std::unordered_map<string, string> etc;
public:
  MysqlClient()
  {
  }
  bool MysqlConnect()
  {
    if(!mysql_real_connect(con, etc["host"].c_str(), etc["user"].c_str(), \
      etc["passwd"].c_str(), etc["db"].c_str(), atoi(etc["port"].c_str()), nullptr, 0))
    {
      cerr << "mysql connect failed." << endl;
      return false;
    }
    return true;
  }
  void MysqlClose()
  {
    mysql_close(con);
  }
  void InitMysqlClinet()
  {
    Util::Load(DB_ETC, etc);
    con = mysql_init(nullptr);
    if(con == nullptr)
    {
      cerr << "mysql init failed." << endl;
      exit(EXIT_FAILURE);
    }
    MysqlConnect();
    //指定客户端与服务器之间传递字符的编码规则为utf8
    if(0 != mysql_query(con, "set names 'utf8'"))
    {
      cerr << "set names 'utf8' failed." << endl;
    }
    MysqlClose();
  }
  bool InsertUser(const string& name, const string& passwd)
  {
    MysqlConnect();
    string query("INSERT INTO user VALUES (\"");
    query += (name + "\", \"" + passwd + "\")");
    int ret = mysql_query(con, query.c_str());
    MysqlClose();
    if(ret != 0)
        return false;
    return true;
  }
  bool SelectUser(const string& username, const string& passwd)
  {
    MysqlConnect();
    string query("SELECT * FROM user WHERE username=");
    query += (username + " AND " + "password=" + passwd);
    int ret = mysql_query(con, query.c_str());
    MysqlClose();
    if(ret != 0) 
    {
      cerr << "mysql_query failed!" << endl;
      return false;
    }
    MYSQL_RES *select_ret = mysql_store_result(con);
    my_ulonglong rows = mysql_num_rows(select_ret);
    if(rows == 0)
    {
      return false;
    }
    return true;
  }
  
};
