MYFLAGS = -MMD -g -Wall -pthread

TAR_DIR = lib/
SRC_DIR = src/
OBJ_DIR = obj/
RELEASE_INC_DIR = ../../release/include/
RELEASE_LIB_DIR = ../../release/lib/

TARGET = $(TAR_DIR)libbayonet.a

INC_DIRS = $(shell find ./src/ -type d | grep -v ".svn")

INC = $(patsubst %, -I%, $(INC_DIRS))

SOURCES = $(shell find src/ -name *.cpp)

OBJS = $(patsubst $(SRC_DIR)%.cpp, $(OBJ_DIR)%.o, $(SOURCES))

OBJ_SUB_DIRS = $(sort $(dir $(OBJS)))

DEPENDS = $(patsubst %.o, %.d, $(OBJS))

.PHONY: all clean init

all: init $(TARGET)
	find . -name "*.h" | xargs -If cp f $(RELEASE_INC_DIR)
	cp $(TARGET) $(RELEASE_LIB_DIR)
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
	mkdir -p $(sort ./ $(TAR_DIR) $(SRC_DIR) $(OBJ_DIR) $(OBJ_SUB_DIRS) $(RELEASE_INC_DIR) $(RELEASE_LIB_DIR))

-include $(DEPENDS)
