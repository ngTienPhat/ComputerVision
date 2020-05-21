CC = "g++"
DCMAKE_CXX_FLAGS="-std=c++11"

PROJECT = bin/1712122_BT03
LIBS = `pkg-config opencv4 --cflags --libs`

BIN := bin 
SRC := src 
INCLUDE := include
CPP_FILE := src/*.cpp
MAIN_FILE := src/main.cpp



$(PROJECT) : $(MAIN_FILE)
	$(CC) $(CPP_FILE) -o $(PROJECT) -I $(INCLUDE) $(LIBS) $(DCMAKE_CXX_FLAGS)