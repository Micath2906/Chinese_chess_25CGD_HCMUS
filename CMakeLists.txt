cmake_minimum_required(VERSION 3.15)

project(CoTuong)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# =========================
# SFML Configuration
# =========================

set(SFML_STATIC_LIBRARIES TRUE)

if(NOT SFML_DIR OR NOT EXISTS "${SFML_DIR}")
    set(SFML_DIR "${CMAKE_SOURCE_DIR}/SFML/lib/cmake/SFML" CACHE PATH "SFML directory" FORCE)
endif()

# Fix upstream SFML 2.6 bug where OpenAL target is referenced but not declared in static export
if(NOT TARGET OpenAL)
    add_library(OpenAL INTERFACE IMPORTED)
    add_library(OpenAL::OpenAL ALIAS OpenAL)
endif()

find_package(SFML 2.6 COMPONENTS graphics window system audio REQUIRED)

# =========================
# Include directories
# =========================

include_directories(
    "${CMAKE_SOURCE_DIR}/include"
)

# =========================
# Source files
# =========================

file(GLOB_RECURSE SOURCES
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
)

# =========================
# Executable
# =========================

add_executable(CoTuong ${SOURCES})

# =========================
# Link libraries
# =========================

target_link_libraries(CoTuong PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
    sfml-audio
)

# =========================
# Copy resources and DLLs
# =========================

add_custom_command(TARGET CoTuong POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/resources"
    "$<TARGET_FILE_DIR:CoTuong>/resources"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/SFML/bin/openal32.dll"
    "$<TARGET_FILE_DIR:CoTuong>/openal32.dll"
)