#pragma once
#include <iostream>

#define SESSION_ARR_SIZE 1024

typedef struct session_info
{
  uint64_t session_id;
  std::string session_name;
  double created_time;
  double last_time;
}session_t;

class Session 
{
private:
  session_t session[SESSION_ARR_SIZE]; //所有session信息
public:
  bool CheckSession();
};