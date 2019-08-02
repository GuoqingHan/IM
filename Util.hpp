#pragma once
#include <string>
#include <unordered_map>
#include <fstream>
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
};