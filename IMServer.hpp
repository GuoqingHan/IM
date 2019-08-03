#pragma once
#include <iostream>
#include "mongoose.h"
#include "MysqlClient.hpp"
//#include "Util.hpp"

using std::cout;
using std::endl;

struct mg_serve_http_opts s_http_server_opts;
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
    static void boardcast(struct mg_connection *_nc, const string msg)
    {
      struct mg_connection *c; //遍历链表的临时变量
      for(c = mg_next(_nc->mgr, NULL); c != NULL; c = mg_next(_nc->mgr, c))
      {
        mg_send_websocket_frame(c, WEBSOCKET_OP_TEXT, msg.c_str(), msg.size());
        //向c发送msg，信息类型为WEBSOCKET_OP_TEXT
        //底层会将其封装为报文格式放入发送缓冲区
      }
    }
    static void ev_hander(struct mg_connection *_nc, int ev, void *ev_data)
    {
      switch(ev)
      {
        case MG_EV_HTTP_REQUEST:
        {
          mg_serve_http(_nc, (struct http_message *)ev_data, s_http_server_opts);
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
        case MG_EV_WEBSOCKET_HANDSHAKE_DONE:
        {
          string msg = "有新人加入...";
          boardcast(_nc, msg);
        }
        case MG_EV_WEBSOCKET_FRAME: //New websocket message.
        {
          struct websocket_message *wm = (struct websocket_message *)ev_data;
          struct mg_str d = {(char *) wm->data, wm->size};
          string msg = Util::mg_str_2_string(d);
          boardcast(_nc, msg);
        }
        default:
          break;
      }
    }
    static void login_hander(struct mg_connection *_nc, int ev, void *ev_data)
    {
      struct http_message *hm = (struct http_message *)ev_data;
      
      cout << "event: " << ev << " body: " << Util::mg_str_2_string(hm->body);
    }
    static void signup_hander(struct mg_connection *_nc, int ev, void *ev_data)
    {

    }
    void InitServer()
    {
      mg_mgr_init(&mgr, NULL);
      nc = mg_bind(&mgr, s_http_port.c_str(), ev_hander); //address可选为仅端口号，也可以是IP:port
      mg_set_protocol_http_websocket(nc); //将内置的HTTP事件处理程序附加到给定连接，nc收到新的连接一开始为HTTP事件，是这样吗
      s_http_server_opts.document_root = "./web/";
      //s_http_server_opts.index_files = "index.html";
      s_http_server_opts.enable_directory_listing = "yes"; //是否禁用目录列表
      mg_register_http_endpoint(nc, "/login", login_hander);
      //为指定的http端点指定回调，注意:如果注册了回调函数，它将被调用，而不是，mg_bind中提供的回调
      mg_register_http_endpoint(nc, "/signin", signup_hander);
    }
    void Run()
    {
      printf("Started on port %s\n", s_http_port.c_str());
      while(1)
      {
        mg_mgr_poll(&mgr, 1000); //第二个参数为等待时间，ms
        //检查所有连接的IO就绪情况，有事件就绪，则触发响应的事件处理程序并返回。
        //监听套接字的事件处理程序为ev_hander
      }
    }
    ~IMServer()
    {
      mg_mgr_free(&mgr);
    }
};


//1.websocket长连接为什么会出现连接关闭事件
//2.为什么会在聊天界面打印一条ws请求报文