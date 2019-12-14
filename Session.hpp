#pragma once
#include <iostream>

#define SESSION_ARR_SIZE 1024
#define SESSION_LIFE_CYCLE 1800.0
#define SESSION_CHECK_TIME 10.0
#define SESSION_ID "im_sid"
#define SESSION_NAME "im_sname"

typedef struct session_info
{
  uint64_t session_id;
  std::string session_name; //session所属用户名
  double created_time;
  double last_use_time; //最后一次使用时间
}session_t;

class Session {
private:
  session_t sessions[SESSION_ARR_SIZE]; //用于存储服务器端session信息
public:
  Session() {
    for(int i = 0; i < SESSION_ARR_SIZE; ++i) {
      sessions[i].session_id = 0;
    }
  }

  bool IsExistSession(http_message *hm) {
    char buf[64];
    char *pbuf = buf;
    uint64_t sid;
    //查找是否存在cookie字段，存在返回内容为"cookie"，不存在返回nullptr
    struct mg_str *cookie_header = mg_get_http_header(hm, "cookie");
    if(cookie_header == nullptr) {
      return false;
    }
    //查找cookie字段中是否有session_id信息，有则放入buf，返回值为buf中有效字符个数
    int ret = mg_http_parse_header2(cookie_header, SESSION_ID, &pbuf, sizeof(buf));
    if(ret == 0) {
      return false;
    }
    sid = strtoull(buf, NULL, 10); //str-->unsignedlonglong(uint64_t) 10进制
    for(int i = 0; i < SESSION_ARR_SIZE; ++i) {
      if(sessions[i].session_id == sid) {
        sessions[i].last_use_time = mg_time(); //更新last_use_time
        return true;
      }
    }
    return false;
  }

  bool CreateSession(const string& name, uint64_t& session_id) { //创建失败返回0值
    int i = 0; 
    for(; i < SESSION_ARR_SIZE; ++i) { //找到一个空的位置
      if(sessions[i].session_id == 0) {
        break;
      }
    }
    if(i != SESSION_ARR_SIZE) {
      sessions[i].session_id = (uint64_t)(mg_time() * 1000000L);
      sessions[i].session_name = name;
      sessions[i].created_time = sessions[i].last_use_time = mg_time();

      session_id = sessions[i].session_id;
      return true;
    }
    return false;
  }

  void CheckSession() { //检查session是否失效
    double keepalive = mg_time() - SESSION_LIFE_CYCLE; 
    //last_use_time在keepalive时间点之后的session保存，之前的删除
    for(int i = 0; i < SESSION_ARR_SIZE; ++i) {
      if(sessions[i].session_id != 0 && sessions[i].last_use_time < keepalive) {
        DestorySession(sessions+i);
      }
    }
  }

  void DestorySession(session_t *s) {
    s->session_id = 0;
  }

  ~Session() = default;
};