* run.cpp: 服务器main函数
* Server.cpp, Server.hpp: 负责网络连接(epoll), 消息收发, 递交玩家请求至游戏逻辑进程
* Session.hpp, Session.cpp: 连接会话类
* Message.hpp, Message.cpp: C/S消息定义
* MsgHandler.cpp, MsgHandler.hpp: 消息处理函数
* Fifo.hpp, Fifo.cpp: 进程间消息通信的封包解包
* User.cpp, User.hpp: 玩家类
* Round.cpp, Round.hpp: 战局类
* UserGesture.hpp: 手势定义(剪刀石头布)
* NetworkUtils.hpp, NetworkUtils.cpp: IO辅助函数

* client.cpp: 客户端

* Makefile