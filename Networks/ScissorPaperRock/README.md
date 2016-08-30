# 猜拳网络小游戏

利用`epoll`实现的猜拳网络小游戏，支持战局管理、心跳检测、超时管理，分离连接管理和游戏逻辑进程

## Usage：
`make`编译，`./run`启动服务器，另开终端`./client`启动客户端

## 客户端支持的玩家操作
* 登录服务器，例如: `login [username]`
* 查看在线用户信息，包括用户名，当前状态，得分等。例如: `info [all | username]`
* 发起一个战局，例如: `open [roundname]`
* 加入一个战局，例如: `join [roundname]`
* 出招，例如: `cast [scissor | paper | rock]`
* 放弃，例如: `quit`
* 登出服务器，例如: `logout`

## SRC结构：

* `run.cpp`: 服务器`main`函数
* `Server.cpp`, `Server.hpp`: 负责网络连接管理(`epoll`), 心跳检测, 消息收发, 超时检测, 与游戏逻辑进程通信
* `Session.hpp`, `Session.cpp`: 连接会话类
* `Timer.cpp`, `Timer.hpp`: 定时器类, 负责服务器所有定时任务
* `Message.hpp`, `Message.cpp`: C/S和进程间消息定义
* `MsgHandler.cpp`, `MsgHandler.hpp`: 消息处理函数
* `Fifo.hpp`, `Fifo.cpp`: 进程间消息通信的封包解包
* `User.cpp`, `User.hpp`: 玩家类
* `Round.cpp`, `Round.hpp`: 战局类
* `UserGesture.hpp`: 手势定义(剪刀石头布)
* `NetworkUtils.hpp`, `NetworkUtils.cpp`: IO辅助函数

* `client.cpp`: 客户端

* `Makefile`
