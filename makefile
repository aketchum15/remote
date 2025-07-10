# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++23 -g 

# ALSA library
LIBS = -lasound

TARGET = remote

# Source and target
SRCS = main.cxx recorder.cxx

OBJS := $(SRCS:.cxx=.o)


# Default target
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

%.o: %.cxx
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build artifacts
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all  clean rebuild
