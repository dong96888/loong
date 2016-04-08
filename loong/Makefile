CC=/usr/local/bin/g++ -std=c++11
FLAGS=-g -Wall

LIBS=-lpthread -lrt -L../lib -lmysqlclient_r ../lib/libmysqlcppconn-static.a

INCS=-I. -I./base -I./net -I./utils -I./db -I../include

VPATH=base:net:db:utils:utils/polarssl

OBJ_DIR=./.obj
BIN_DIR=../bin

SRCS=$(wildcard *.cpp base/*.cpp net/*.cpp db/*.cpp utils/*.cpp)
OBJS=$(patsubst %.cpp, $(OBJ_DIR)/%.o, $(notdir ${SRCS}) )

C_SRCS=$(wildcard utils/polarssl/*.c)
C_OBJS=$(patsubst %.c, $(OBJ_DIR)/%.o, $(notdir ${C_SRCS}))

TARGET=../lib/libloong.a 
$(BIN_DIR)/$(TARGET):$(OBJS) $(C_OBJS)
	ar -cr $@ $^ 
	
$(OBJ_DIR)/%.o:%.cpp
	$(CC) -c $(FLAGS) $(INCS) $< -o $@
	
$(OBJ_DIR)/%.o:%.c
	$(CC) -c $(FLAGS) $(INCS) $< -o $@

clean:
	-rm -f $(OBJ_DIR)/*.o



