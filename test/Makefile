CC=clang++ -std=c++11
FLAGS=-g -Wall

LIBS=-lpthread -lrt -L/usr/lib64/mysql -lmysqlclient_r ../lib/libmysqlcppconn-static.a -L../lib -lloong

INCS=-I. -I../loong/base -I../loong/net -I../loong/utils -I../loong/db -I../include

#VPATH=../loong/base:../loong/net:../loong/db:../loong/utils

SRCS=$(wildcard *.cpp)

OBJ_DIR=./.obj
OBJS=$(patsubst %.cpp, $(OBJ_DIR)/%.o, $(notdir ${SRCS}) )

BIN_DIR=./

TARGET=timer 

$(BIN_DIR)/$(TARGET):$(OBJS)
	$(CC) $(FLAGS) -o $@ $^ $(LIBS) 
$(OBJ_DIR)/%.o:%.cpp
	$(CC) -c $(FLAGS) $(INCS) $< -o $@

clean:
	-rm -f $(OBJ_DIR)/*.o



