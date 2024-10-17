# Makefile for Chat Program

CXX = g++
CXXFLAGS = -Wall -std=c++11
LDFLAGS = -lpthread

# 文件名定义
CLIENT_SRC = client.cpp protocol.h utils.h utils.cpp
SERVER_SRC = server.cpp protocol.h utils.h utils.cpp
CLIENT_OBJ = $(CLIENT_SRC:.cpp=.o)
SERVER_OBJ = $(SERVER_SRC:.cpp=.o)

# 可执行文件
CLIENT_EXEC = chat_client
SERVER_EXEC = chat_server

all: $(CLIENT_EXEC) $(SERVER_EXEC)

$(CLIENT_EXEC): $(CLIENT_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $(CLIENT_OBJ) $(LDFLAGS)

$(SERVER_EXEC): $(SERVER_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $(SERVER_OBJ) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.o $(CLIENT_EXEC) $(SERVER_EXEC)

.PHONY: all clean
