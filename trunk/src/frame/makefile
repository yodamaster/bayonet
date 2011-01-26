SRC_DIR= src/
OBJ_DIR= obj/
LIB_DIR= lib/

OBJ_EXT= .o
CXXSRC_EXT= .cpp
CSRC_EXT= .c
LIB_EXT= .a
H_EXT= .h
INC+=-I./src/fsm

OBJECTS = $(OBJ_DIR)socketactor_set$(OBJ_EXT)\
		  $(OBJ_DIR)socketactor_base$(OBJ_EXT)\
		  $(OBJ_DIR)epoller$(OBJ_EXT)\
		  $(OBJ_DIR)bayonet_frame$(OBJ_EXT)\


LIB_TARGET = $(LIB_DIR)libbayonet$(LIB_EXT)

$(OBJ_DIR)%$(OBJ_EXT): $(SRC_DIR)%$(CXXSRC_EXT)
	@echo
	@echo "Compiling $< ==> $@..."
	$(CXX) $(INC) $(C_FLAGS) -c $< -o $@

$(OBJ_DIR)%$(OBJ_EXT): $(SRC_DIR)%$(CSRC_EXT)
	@echo
	@echo "Compiling $< ==> $@..."
	$(CC)  $(INC) $(C_FLAGS) -c $< -o $@

all:$(LIB_TARGET)

$(LIB_TARGET): $(OBJECTS)
	@echo
	$(AR) rc $(LIB_TARGET) $(OBJECTS)
	@echo "ok"

clean:
	rm -f $(LIB_TARGET) $(OBJECTS)
