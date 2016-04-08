CC=g++ --std=c++11
FLAGS=-g -Wall

LIBS=-lpthread -lrt -L/usr/lib64/mysql -lmysqlclient /usr/lib64/libmysqlcppconn-static.a

INCS=-I../../include -I../comm

VPATH=../comm

SRCS=$(wildcard *.cpp ../comm/*.cpp)

OBJ_DIR=./obj
OBJS=$(patsubst %.cpp, $(OBJ_DIR)/%.o, $(notdir ${SRCS}) )

BIN_DIR=./bin

TARGET=gateway 

$(BIN_DIR)/$(TARGET):$(OBJS)
	$(CC) $(FLAGS) -o $@ $^ $(LIBS) 
$(OBJ_DIR)/%.o:%.cpp
	$(CC) -c $(FLAGS) $(INCS) $< -o $@

clean:
	-rm -f $(OBJ_DIR)/*.o



