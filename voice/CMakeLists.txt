cmake_minimum_required(VERSION 3.16)
project(voice CXX)

# Set the C++ standard and flags
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add custom warning flags (equivalent to -Wall -Wextra -g)
add_compile_options(-Wall -Wextra -g)

# Include directories
include_directories(inc)

# Collect all source files from src/
file(GLOB SOURCES src/*.cxx)

# Define the executable target
add_executable(voice ${SOURCES})

# Link against required libraries
target_link_libraries(voice
    asound
    opus
    gpiodcxx
)

install(TARGETS voice RUNTIME DESTINATION bin )
