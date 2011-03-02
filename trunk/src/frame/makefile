MYFLAGS = -MMD -g -Wall -pthread

INC += -I./src/fsm

TAR_DIR = lib/
SRC_DIR = src/
OBJ_DIR = obj/

TARGET = $(TAR_DIR)libbayonet.a

OBJS = \
	   $(OBJ_DIR)socketactor_base.o\
	   $(OBJ_DIR)socketactor_data.o\
	   $(OBJ_DIR)socketactor_listen_tcp.o\
	   $(OBJ_DIR)socketactor_listen_udp.o\
	   $(OBJ_DIR)epoller.o\
	   $(OBJ_DIR)bayonet_frame.o\
	   $(OBJ_DIR)fl_log.o\
	   $(OBJ_DIR)net_handler.o\
	   $(OBJ_DIR)socketactor_passive.o\
	   $(OBJ_DIR)action_info.o\
	   $(OBJ_DIR)appactor_base.o\


OBJ_SUB_DIRS = $(sort $(dir $(OBJS)))

DEPENDS = $(patsubst %.o, %.d, $(OBJS))

.PHONY: all clean init

all: init $(TARGET)
	@echo -e "\033[1;32m\nSuccess!\033[0m"

$(TARGET): $(OBJS)
	@echo -e "\033[1;33m\nLinking $< ==> $@\033[0m"
	$(AR) rc $(TARGET) $(OBJS)

$(OBJ_DIR)%.o: $(SRC_DIR)%.c
	@echo -e "\033[1;33m\nCompiling $< ==> $@\033[0m"
	$(CC) $(MYFLAGS) -c -o $@ $(INC) $<

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp
	@echo -e "\033[1;33m\nCompiling $< ==> $@\033[0m"
	$(CXX) $(MYFLAGS) -c -o $@ $(INC) $<

clean:
	rm -f $(OBJS) $(TARGET) $(DEPENDS)

init:
	mkdir -p $(sort ./ $(TAR_DIR) $(SRC_DIR) $(OBJ_DIR) $(OBJ_SUB_DIRS))

-include $(DEPENDS)
