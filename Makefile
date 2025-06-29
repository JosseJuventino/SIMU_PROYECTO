CXX = g++
CXXFLAGS = -std=c++17 -m64 -Iinclude
LDFLAGS  = -m64 -L"C:/SFML/lib" -lsfml-graphics -lsfml-window -lsfml-system

SRC_DIR   = src
BUILD_DIR = build

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))
BIN  = EscapeGrid.exe

all: $(BIN)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -Iinclude -c $< -o $@

$(BIN): $(OBJS)
	$(CXX) $(OBJS) $(LDFLAGS) -o $(BIN)

$(BUILD_DIR):
	mkdir build

clean:
	del /Q $(BUILD_DIR)\*.o $(BIN)

.PHONY: all clean
