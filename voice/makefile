# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++23 -g 


# Define dirs 
SRC_DIR   = src
INC_DIR   = inc 
BUILD_DIR = build

# libraries and include
LIBS = -lasound -lopus -lgpiodcxx
INC = -I$(INC_DIR)

# Source and target
SRCS := $(wildcard src/*)
OBJS := $(patsubst $(SRC_DIR)/%.cxx, $(BUILD_DIR)/%.o, $(SRCS))

TARGET = $(BUILD_DIR)/remote

# Default target
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cxx
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(INC)

# Clean up build artifacts
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all  clean rebuild
