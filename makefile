# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17

# ALSA library
LIBS = -lasound

# Source and target
SRC = main.cxx
TARGET = remote

# Default target
all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

# Clean up build artifacts
clean:
	rm -f $(TARGET)
