#pragma once
#include <iostream>
#include <string>
#include "mongoose/mongoose.h"

using std::cout;
using std::endl;

class IMServer
{
  private:
    struct mg_mgr mgr;
    std::string s_http_port;
    struct mg_connection *nc; //监听socket
  public:
    IMServer(std::string _port = "8080"):s_http_port(_port)
    {

    }
    static void ev_hander(struct mg_connection *nc, int ev, void *ev_data)
    {
      switch(ev)
      {
        case MG_EV_HTTP_REQUEST:
        {
          cout << "hello" << endl;
          struct mg_serve_http_opts s_http_server_opts;
          s_http_server_opts.document_root = ".";
          //s_http_server_opts.index_files = "index.html";
          s_http_server_opts.enable_directory_listing = "yes"; //是否禁用目录列表
          mg_serve_http(nc, (struct http_message *)ev_data, s_http_server_opts); //段错误
          cout << "hello" << endl;
          //_nc:客户端（发起请求方）套接字信息
          //(http_message *)ev_data:请求报文信息
          //s_http_server_opts:服务器端响应选项，对于http请求，响应什么
          break;
        }
        case MG_EV_CLOSE:
        {
          cout << "Client quit." << endl;
          break;
        }
        default:
          break;
      }
    }
    void InitServer()
    {
      mg_mgr_init(&mgr, NULL);
      nc = mg_bind(&mgr, s_http_port.c_str(), ev_hander); //address可选为仅端口号，也可以是IP:port
      mg_set_protocol_http_websocket(nc); //将内置的HTTP事件处理程序附加到给定连接，nc收到新的连接一开始为HTTP事件，是这样吗
    }
    void Run()
    {
      printf("Started on port %s\n", s_http_port.c_str());
      while(1)
      {
        mg_mgr_poll(&mgr, 200); //第二个参数为等待时间，ms
        //检查所有连接的IO就绪情况，有事件就绪，则触发响应的事件处理程序并返回。
        //监听套接字的事件处理程序为ev_hander
      }
    }
    ~IMServer()
    {
      mg_mgr_free(&mgr);
    }
};