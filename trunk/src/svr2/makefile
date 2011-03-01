CC = gcc
CXX = g++

MYFLAGS = -MMD -g -Wall -pthread

INC+=-I../frame/src -I../frame/src/fsm
LIB+=-L../frame/lib -lbayonet

TAR_DIR= ./
SRC_DIR= ./
OBJ_DIR= obj/

TARGET = $(TAR_DIR)svr2

OBJS = \
	   $(OBJ_DIR)main.o \


DEPENDS = $(patsubst %.o, %.d, $(OBJS))

.PHONY: all clean prepare

all: prepare $(TARGET)
	@echo -e "\033[1;32m\nSuccess!\033[0m"

$(TARGET): $(OBJS)
	@echo -e "\033[1;33m\nLinking $< ==> $@\033[0m"
	$(CXX) -o $@ $^ $(LIB) $(MYFLAGS)

$(OBJ_DIR)%.o: $(SRC_DIR)%.c
	@echo -e "\033[1;33m\nCompiling $< ==> $@\033[0m"
	$(CC) $(MYFLAGS) -c -o $@ $(INC) $<

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp
	@echo -e "\033[1;33m\nCompiling $< ==> $@\033[0m"
	$(CXX) $(MYFLAGS) -c -o $@ $(INC) $<

clean:
	rm -f $(OBJS) $(TARGET) $(DEPENDS)

prepare:
	mkdir -p $(TAR_DIR) $(SRC_DIR) $(OBJ_DIR)

-include $(DEPENDS)
