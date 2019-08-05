#pragma once
#include <iostream>
#include "mongoose.h"
#include "MysqlClient.hpp"
#include "Session.hpp"
#include <json/json.h>
#include <unordered_map>

using std::cout;
using std::endl;

struct mg_serve_http_opts s_http_server_opts;
class IMServer
{
  private:
    struct mg_mgr mgr;
    std::string s_http_port;
    struct mg_connection *nc; //监听socket
    static MysqlClient sql;
  public:
    IMServer(std::string _port = "8080"):s_http_port(_port)
    {
    }

    //广播消息
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

    //向listen_socket注册的事件处理函数
    static void ev_hander(struct mg_connection *_nc, int ev, void *ev_data)
    {
      switch(ev)
      {
        //HTTP请求事件
        case MG_EV_HTTP_REQUEST:
        {
          mg_serve_http(_nc, (struct http_message *)ev_data, s_http_server_opts); //请求什么返回什么
          //_nc:客户端（发起请求方）套接字信息
          //(http_message *)ev_data:请求报文信息
          //s_http_server_opts:服务器端响应选项，对于http请求，响应什么


          _nc->flags |= MG_F_SEND_AND_CLOSE; //响应后关闭连接
          break;
        }
        //连接关闭事件
        case MG_EV_CLOSE:
        {
          //cout << "Client quit." << endl;
          break;
        }
        //升级websocket协议已完成
        case MG_EV_WEBSOCKET_HANDSHAKE_DONE:
        {
          string msg = "有新人加入...";
          boardcast(_nc, msg);
        }
        //新消息事件
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
    //登录事件处理函数
    static void signin_hander(struct mg_connection *_nc, int ev, void *ev_data)
    {
      if(ev == MG_EV_CLOSE) { //这个是什么连接关闭，会到这里？
        return;
      }
      cout << "signin" << endl;
      struct http_message *hm = (struct http_message *)ev_data;
      string method = Util::mg_str_2_string(hm->method);
      if(method == "POST") {
        string jsonstr = Util::mg_str_2_string(hm->body);
        //jsonstr --> {"name":"1","passwd":"123"}
        std::unordered_map<string, string> kv; //name passwd
        if(!Util::Json2Map(jsonstr, kv))
          return;
        string code = "0";    
        string body_json = "{\"result\":";
        //用户名密码不能为空
        if(!kv["name"].empty() && !kv["passwd"].empty()) {
          bool ret = sql.SelectUser(kv["name"], kv["passwd"]);
          mg_printf(_nc, "HTTP/1.1 200 OK\r\n");
          if(ret) { //存在此用户
            cout << "true" << endl;
            //在服务器端创建session，返回session_id

            //返回code = 0，浏览器可跳转到主页
            code = "0";
          }
          else { //不存在此用户
            code = "1";
          }
        } else { //用户名或密码为空
          code = "2";
        }
        body_json += (code + "}");
        //存在正文，填充Content-Length字段
        mg_printf(_nc, "Content-Length: %u\r\n\r\n", body_json.size()); 
        mg_printf(_nc, body_json.data());
      } else { //请求方法不是POST
        mg_serve_http(_nc, hm, s_http_server_opts);
      }
      _nc->flags |= MG_F_SEND_AND_CLOSE; //响应完成后关闭连接
    }
    //注册事件处理函数
    static void signup_hander(struct mg_connection *_nc, int ev, void *ev_data)
    {
      cout << "signup" << endl;
      //点击注册会阻塞在这里，再次点击才会打印信息
      struct http_message *hm = (struct http_message *)ev_data;
      cout << "event: " << ev << " body: " << Util::mg_str_2_string(hm->body) << endl;
    }
    void InitServer()
    {
      mg_mgr_init(&mgr, NULL); //会忽略掉SIGPIPE
      //绑定port，注册事件处理方法
      nc = mg_bind(&mgr, s_http_port.c_str(), ev_hander); //address可选为仅端口号，也可以是IP:port
      mg_set_protocol_http_websocket(nc); //将内置的HTTP事件处理程序附加到给定连接，nc收到新的连接一开始为HTTP事件，是这样吗
      s_http_server_opts.document_root = "./www/";
      //s_http_server_opts.index_files = "index.html";
      s_http_server_opts.enable_directory_listing = "yes"; //是否禁用目录列表
      //为指定的http端点指定回调，注意:如果注册了回调函数，它将被调用，而不是，mg_bind中提供的回调
      //注册访问登录页面时的处理方法
      mg_register_http_endpoint(nc, "/IN", signin_hander);
      //注册访问注册页面时的处理方法
      mg_register_http_endpoint(nc, "/UP", signup_hander);
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

MysqlClient IMServer::sql;


//1.websocket长连接为什么会出现连接关闭事件
//2.为什么会在聊天界面打印一条ws请求报文

//注意：每一个连接都有一个读缓冲区和写缓冲区

//mgr --> iface

//Cookie Session MySQL