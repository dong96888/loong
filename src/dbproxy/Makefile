CC=g++
FLAGS=-g -Wall

LIBS=-lpthread -lrt

INCS=-I. -I../Comm

VPATH=../Comm

SRCS=$(wildcard *.cpp ../Comm/*.cpp)

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



