CC=/usr/local/bin/g++ -std=c++11
FLAGS=-g -Wall

LIBS=-lpthread -lrt -L../../lib -lloong ../../lib/libloong.a

INCS=-I. -I../../loong/base -I../../loong/net -I../../include -I../../loong/db 

VPATH=.

SRCS=$(wildcard *.cpp)

OBJ_DIR=./obj
OBJS=$(patsubst %.cpp, $(OBJ_DIR)/%.o, $(notdir ${SRCS}) )

BIN_DIR=./bin

TARGET=cli 

$(BIN_DIR)/$(TARGET):$(OBJS)
	$(CC) $(FLAGS) -o $@ $^ $(LIBS) 
$(OBJ_DIR)/%.o:%.cpp
	$(CC) -c $(FLAGS) $(INCS) $< -o $@

clean:
	-rm -f $(OBJ_DIR)/*.o



