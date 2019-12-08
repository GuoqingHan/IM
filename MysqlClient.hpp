#pragma once
#include "mysql.h"
#include "Util.hpp"
#include <cstdio>
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
    con = mysql_init(nullptr);
    if(con == nullptr)
    {
      cerr << "mysql init failed." << endl;
      return false;
    }
    Util::Load(DB_ETC, etc);
    if(!mysql_real_connect(con, etc["host"].data(), etc["user"].data(), \
      etc["passwd"].data(), etc["db"].data(), atoi(etc["port"].data()), nullptr, 0))
    {
      cerr << "mysql connect failed." << endl;
      return false;
    }
    //指定客户端与服务器之间传递字符的编码规则为utf8
    if(0 != mysql_query(con, "set names 'utf8'"))
    {
      cerr << "set names 'utf8' failed." << endl;
    }
    return true;
  }

  void MysqlClose()
  {
    mysql_close(con);
  }

  bool InsertUser(const string& name, const string& passwd)
  {
    MysqlConnect();
    string query("INSERT INTO user VALUES (\"");
    query += (name + "\", \"" + passwd + "\")");
    int ret = mysql_query(con, query.data());
    MysqlClose();
    if(ret != 0)
        return false;
    return true;
  }

  bool SelectUser(const string& username, const string& passwd)
  {
    if(!MysqlConnect())
    {
      cerr << "MysqlConncet failed!" << endl;
      return false;
    }
    string query("SELECT * FROM user WHERE username=\"");
    query += (username + "\" AND " + "password=\"" + passwd + "\"");
    //cout << query << endl;
    int ret = mysql_query(con, query.data());
    if(ret != 0) 
    {
      cerr << "mysql_query failed!" << endl;
      return false;
    }
    MYSQL_RES *select_ret = mysql_store_result(con);
    my_ulonglong rows = mysql_num_rows(select_ret);
    if (select_ret != NULL) {
        mysql_free_result(select_ret); //use free() -> crash
        select_ret = NULL;
    }
    MysqlClose();
    return rows > 0;
  }
};
