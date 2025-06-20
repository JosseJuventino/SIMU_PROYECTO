# Makefile para Windows (MSYS2 MinGW-64) + SFML (64-bit)
CXX      := g++
CXXFLAGS := -std=c++17 -m64 -Iinclude -I/mingw64/include
LDFLAGS  := -m64 -L/mingw64/lib -lsfml-graphics -lsfml-window -lsfml-system

# Toma todos los .cpp de src
SRC  := $(wildcard src/*.cpp)
OBJ  := $(patsubst src/%.cpp,build/%.o,$(SRC))
BIN  := EscapeGrid.exe

all: $(BIN)

# Compila cada .cpp a .o
build/%.o: src/%.cpp
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Enlaza todos los .o
$(BIN): $(OBJ)
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $@

clean:
	rm -rf build $(BIN)

.PHONY: all clean
