#pragma once
#include <string>
using std::string;
using std::cerr;
using std::endl;

class Util
{
public:
  static string mg_str_2_string(mg_str& msg)
  {
    string ret(msg.p, msg.len);
    return ret;
  }
};