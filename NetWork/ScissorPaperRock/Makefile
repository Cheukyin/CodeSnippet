all: server client

server: Message.cpp User.cpp Server.cpp Round.cpp MsgHandler.cpp run.cpp Session.cpp User.hpp Server.hpp Round.hpp MsgHandler.hpp Session.hpp Message.hpp UserGesture.hpp
	clang++ -O0 -Wall -std=c++11 -g3 -ggdb User.cpp Server.cpp Round.cpp MsgHandler.cpp run.cpp Session.cpp Message.cpp -o server

client: client.cpp NetworkUtils.hpp Message.hpp Message.cpp UserGesture.hpp
	clang++ -O0 -std=c++11 -W -g3 -ggdb client.cpp Message.cpp -o client

clean:
	rm server client
