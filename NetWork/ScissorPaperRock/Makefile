CXX = clang++
CXXFLAGS += -O0 -Wall -std=c++11 -g3 -ggdb
CXX11FLAGS = -std=c++11

SOURCES = $(wildcard *.cpp)

ServerSRC = $(patsubst client.cpp, ,$(SOURCES))
ServerOBJS = $(ServerSRC:.cpp=.o)

ClientSRC = client.cpp NetworkUtils.cpp Message.cpp Timer.cpp
ClientOBJS = $(ClientSRC:.cpp=.o)


all: server client

server: $(ServerOBJS)
	$(CXX) $^ -o $@

client: $(ClientOBJS)
	$(CXX) $^ -o $@


sinclude $(SOURCES:.cpp=.d)
# $$ is process id
# sed is for adding %.d to %.o's dependencies
%.d: %.cpp
	$(CXX) $(CXX11FLAGS) -MM $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o: $@ ,g' < $@.$$$$ > $@; \
	$(RM) $@.$$$$


.PHONY: clean
clean:
	-rm -f *.o *.d server client