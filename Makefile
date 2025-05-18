SRC_DIR := src
OBJ_DIR := obj
INC_DIR := include

SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

LDFLAGS := 
CPPFLAGS := -I$(INC_DIR) 
CXXFLAGS := -std=c++20 -Wall -O2 

main.exe: $(OBJ_FILES)
	g++ $(LDFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	g++ $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<


CXXFLAGS += -MMD
-include $(OBJ_FILES:.o=.d)