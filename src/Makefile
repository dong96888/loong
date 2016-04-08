CC=g++ -std=c++11
FLAGS=-g -Wall

LIBS=-lpthread -lrt -L../../lib -lmysqlclient ../../lib/libmysqlcppconn-static.a

INCS=-I. -I./base -I./net -I./utils -I./db -I../../include

VPATH=comm

SRCS=$(wildcard *.cpp base/*.cpp net/*.cpp db/*.cpp utils/*.cpp)

OBJ_DIR=./obj
OBJS=$(patsubst %.cpp, $(OBJ_DIR)/%.o, $(notdir ${SRCS}) )

BIN_DIR=./bin

TARGET=libloong.a 

$(BIN_DIR)/$(TARGET):$(OBJS)
	ar -cr $@ $^ 
$(OBJ_DIR)/%.o:%.cpp
	$(CC) -c $(FLAGS) $(INCS) $< -o $@

clean:
	-rm -f $(OBJ_DIR)/*.o



