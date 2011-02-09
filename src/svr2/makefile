CXX = g++

TARGET = svr2

C_FLAGS += -g -Wall -pthread
INC+=-I../frame/src -I../frame/src/fsm
LIB+=-L../frame/lib -lbayonet

all: $(TARGET)

svr2:  main.o
	$(CXX) -o $@ $^  $(LIB) $(C_FLAGS)

.cpp.o:
	$(CXX) -c -o $*.o $(INC) $(C_FLAGS) $*.cpp
.cc.o:
	$(CXX) -c -o $*.o $(INC) $(C_FLAGS) $*.cc


clean:
	-rm -f *.o $(TARGET)
