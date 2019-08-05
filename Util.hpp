#pragma once
#include <string>
#include <unordered_map>
#include <fstream>
#include <json/json.h>
#include <memory>
using std::string;
using std::cerr;
using std::cout;
using std::endl;

class Util
{
public:
  static string mg_str_2_string(mg_str& msg)
  {
    string ret(msg.p, msg.len);
    return ret;
  }

  static void Load(string path, std::unordered_map<string, string>& db_etc)
  {
    char buf[256];
    std::ifstream in(path.c_str());
    if(!in.is_open())
    {
        cerr << "Open etc file failed." << endl;
        exit(EXIT_FAILURE);
    }
    string seq = ":";
    while(in.getline(buf, sizeof(buf)))
    {
        string str = buf;
        size_t index = str.find(seq);
        if(index == string::npos)
        {
            cerr << "Seq not found." << endl;
            continue;
        }
        string msg = str.substr(0, index);
        string cmd = str.substr(index+seq.size());
        db_etc[msg] = cmd;
    }
    in.close();
  }

  static bool Json2Map(const string& jsonstr, std::unordered_map<string, string>& kv)
  {
    JSONCPP_STRING errs;
    Json::Value root;
    Json::CharReaderBuilder rb;
    std::unique_ptr<Json::CharReader> const rj(rb.newCharReader());
    bool res = rj->parse(jsonstr.data(), jsonstr.data()+jsonstr.size(), &root, &errs);
    if(!res || !errs.empty())
    {
        cerr << __TIME__ << "Jsonstr parse error: " << errs << endl;
        return false;
    }
    kv["name"] = root["name"].asString();
    kv["passwd"] = root["passwd"].asString();
    return true;
  }
};