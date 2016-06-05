# CXX = g++
# CXXFLAGS += -O0 -Wall -std=c++11 -g3 -ggdb
# CXX11FLAGS = -std=c++11

# SOURCES = $(wildcard *.cpp)

# ServerSRC = $(patsubst client.cpp, ,$(SOURCES))
# ServerOBJS := $(patsubst %.cpp, %.o,$(ServerSRC))

# ClientSRC = client.cpp NetworkUtils.cpp Message.cpp Timer.cpp
# ClientOBJS := $(patsubst %.cpp, %.o,$(ClientSRC))


# all: server client

# server: $(ServerOBJS)
# 	$(CXX) $(ServerOBJS) $(CXXFLAGS) -o server

# client: $(ClientOBJS)
# 	$(CXX) $(ClientOBJS) $(CXXFLAGS) -o client


# sinclude $(SOURCES:.cpp=.d)
# # $$ is process id
# # sed is for adding %.d to %.o's dependencies
# %.d: %.cpp
# 	$(CXX) $(CXX11FLAGS) -MM $< > $@.$$$$; \
# 	sed 's,\($*\)\.o[ :]*,\1.o: $@ ,g' < $@.$$$$ > $@; \
# 	$(RM) $@.$$$$


# .PHONY: clean
# clean:
# 	-rm -f *.o *.d server client

all: server client

server: Message.cpp User.cpp Server.cpp Round.cpp MsgHandler.cpp run.cpp Session.cpp User.hpp Server.hpp Round.hpp MsgHandler.hpp Session.hpp Message.hpp UserGesture.hpp Fifo.hpp Fifo.cpp NetworkUtils.cpp Timer.hpp Timer.cpp
	clang++ -O0 -Wall -std=c++11 -g3 -ggdb User.cpp Server.cpp Round.cpp Fifo.cpp NetworkUtils.cpp MsgHandler.cpp run.cpp Session.cpp Message.cpp Timer.cpp -o server

client: client.cpp NetworkUtils.hpp NetworkUtils.cpp Message.hpp Message.cpp
	clang++ -O0 -std=c++11 -W -g3 -ggdb client.cpp Message.cpp NetworkUtils.cpp -o client

clean:
	rm server client
