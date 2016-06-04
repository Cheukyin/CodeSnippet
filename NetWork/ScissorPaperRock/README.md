* run.cpp: 服务器main函数
* Server.cpp, Server.hpp: 负责网络连接管理(epoll),心跳检测,消息收发,超时检测,与游戏逻辑进程通信
* Session.hpp, Session.cpp: 连接会话类
* Timer.cpp, Timer.hpp: 定时器类, 负责服务器所有定时任务
* Message.hpp, Message.cpp: C/S和进程间消息定义
* MsgHandler.cpp, MsgHandler.hpp: 消息处理函数
* Fifo.hpp, Fifo.cpp: 进程间消息通信的封包解包
* User.cpp, User.hpp: 玩家类
* Round.cpp, Round.hpp: 战局类
* UserGesture.hpp: 手势定义(剪刀石头布)
* NetworkUtils.hpp, NetworkUtils.cpp: IO辅助函数

* client.cpp: 客户端

* Makefile