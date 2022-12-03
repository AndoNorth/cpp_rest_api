CXX		  := g++
CXX_FLAGS := -std=c++17

BIN		:= bin
SRC		:= src/*.cpp $(shell find $(lib) -name *.cpp)
INCLUDE	:= -Iinclude -Ilib
LIB		:= lib

LIBRARIES	:= -lboost_system -lpthread -lmariadbclient
EXECUTABLE	:= restserver


all: $(BIN)/$(EXECUTABLE)

run: clean all
	clear
	./$(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE): $(SRC)
	$(CXX) $(CXX_FLAGS) $(INCLUDE) -L$(LIB) $^ -o $@ $(LIBRARIES)

clean:
	-rm $(BIN)/$(EXECUTABLE)