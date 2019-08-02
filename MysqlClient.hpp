#pragma once
#include "mysql.h"
#include "Util.hpp"
#define DB_ETC "../db.etc"

class MysqlClient
{
private:
  MYSQL *con;
  std::unordered_map<string, string> etc;
public:
  MysqlClient()
  {
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
    if(!mysql_real_connect(con, etc["host"].c_str(), etc["user"].c_str(), \
      etc["passwd"].c_str(), etc["db"].c_str(), atoi(etc["port"].c_str()), nullptr, 0))
    {
      cerr << "mysql connect failed." << endl;
      mysql_close(con);
      exit(EXIT_FAILURE);
    }
    //指定客户端与服务器之间传递字符的编码规则为utf8
    if(0 != mysql_query(con, "set names 'utf8'"))
    {
      cerr << "set names 'utf8' failed." << endl;
    }
  }
  bool InsertUser(const string& name, const string& passwd)
  {
    string query("INSERT INTO user VALUES (\"");
    query += (name + "\", \"" + passwd + "\")");
    if(0 != mysql_query(con, query.c_str()))
      return false;
    return true;
  }
  void SelectUser()
  {
    //mysql_query("SELECT * FROM user");
  }
  
};