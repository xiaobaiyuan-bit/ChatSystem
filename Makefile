cc=g++
server=ChatServer
client=ChatClient
INCLUDE=-I./lib/include
LIB_PATH=-L./lib/lib
LDFLAGS=-pthread -ljson

.PHONY:all
all:$(server) $(client)

$(server):ChatServer.cc
	$(cc) -o $@ $^ $(INCLUDE) $(LIB_PATH) $(LDFLAGS)

$(client):ChatClient.cc
	$(cc) -o $@ $^ $(INCLUDE) $(LIB_PATH) -lncurses $(LDFLAGS)

.PHONY:clean
clean:
	rm -f $(server) $(client)
