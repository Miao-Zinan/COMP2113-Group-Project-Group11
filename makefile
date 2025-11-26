# Makefile for Maze Game
# Text-based ASCII Art version - No external dependencies required

# Detect OS and set compiler
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
    # macOS
    CXX = clang++
else
    # Linux and others
    CXX = g++
endif

CXXFLAGS = -std=c++17 -Wall -O2

# Source files
SOURCES = main_game.cpp \
          GameManager.cpp \
          GameRenderer.cpp \
          InputHandler.cpp \
          Player.cpp \
          ghost.cpp \
          maze_generate.cpp \
          chest_generate.cpp \
          fileio.cpp \
          chest.cpp \
          spawnpoint.cpp

# Object files
OBJECTS = $(SOURCES:.cpp=.o)

# Target executable
TARGET = main

# Default target
all: $(TARGET)

# Build the executable
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET)

# Build object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(OBJECTS) $(TARGET)
	rm -f $(TARGET).exe

# Windows-specific clean
clean-win:
	del /Q *.o $(TARGET).exe 2>nul || exit 0

# Run the game
run: $(TARGET)
	./$(TARGET)

# Windows run
run-win: $(TARGET).exe
	$(TARGET).exe

.PHONY: all clean clean-win run run-win
