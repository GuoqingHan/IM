# 一款基于Mongoose 框架的网页多人聊天工具

### 依赖技术

- <p><a href="https://github.com/cesanta/mongoose">Mongoose</a>(Mongoose Embedded Web Server Library)

- HTTP

- WebSocket

- Session/Cookie

- C++11 STL

- MySQL

- JsonCPP

### 实现细节

​		可通过浏览器直接访问主页，发起HTTP请求，Server检查是否携带SessionID以及检验是否有效，有效则自动登录至聊天页面（主页），否则Server响应浏览器302重定向至登录（注册）页面，没有账号可先注册，Server使用MySQL维护用户信息。注册成功后，跳转至登录界面，用户登录时，Server维护Session信息，并响应SessionID给浏览器，下次便可不用登录访问主页（Session有效期限内），收到响应后，浏览器请求升级协议至WebSocket。

Server使用Mongoose实现对HTTP及WebSocket事件的管理，注册事件处理方法，根据Client请求处理相应事件，涉及事件类型：

- MG_EV_HTTP_REQUEST：收到HTTP请求；
- MG_EV_WEBSOCKET_HANDSHAKE_DONE：WebSocket握手成功，即HTTP至WebSocket协议升级完成；
- MG_EV_WEBSOCKET_FRAME：收到WebSocket数据帧，即新消息事件；
- MG_EV_TIMER：计时器到时，Server维护的计时器，用于维护Session有效期限。

为处理登录和注册相关信息注册了特定的处理方法，不同于其他的HTTP请求事件。

> 具体细节：IMServer.hpp