cmake_minimum_required(VERSION 3.15)
project(CoTuong)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SFML
find_package(SFML 2.5 COMPONENTS graphics window system audio REQUIRED)

# Include directories
include_directories(include)

# Source files
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Create executable
add_executable(CoTuong ${SOURCES})

# Link SFML libraries
target_link_libraries(CoTuong sfml-graphics sfml-window sfml-system sfml-audio)

# Copy resources to build directory
file(COPY ${CMAKE_SOURCE_DIR}/resources DESTINATION ${CMAKE_BINARY_DIR})
