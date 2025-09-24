SRC_DIR := src
OBJ_DIR := obj
INC_DIR := include
BIN_DIR := bin

CXX := g++
CPPFLAGS := -I$(INC_DIR) -I/ucrt64/include/SDL2
CXXFLAGS := -std=c++20 -Wall -O2 -MMD -mwindows
LDFLAGS  := -L/ucrt64/lib -lmingw32 -lSDL2main -lSDL2

SRC_FILES := $(shell find $(SRC_DIR) -name '*.cpp')
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))
TARGET := $(BIN_DIR)/main.exe

$(TARGET): $(OBJ_FILES)
	@mkdir -p $(dir $@)
	$(CXX) -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

-include $(OBJ_FILES:.o=.d)

.PHONY: clean run
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

run: $(TARGET)
	"./$(TARGET)"
