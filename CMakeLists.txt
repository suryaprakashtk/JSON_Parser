cmake_minimum_required(VERSION 3.21)
project(JSON_Parser VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)

# Add sources
FILE(GLOB MY_SOURCES Source/**)

# Create executable
add_executable(JSON_Parser ${MY_SOURCES})

# Set warning level
if (MSVC)
    target_compile_options(JSON_Parser PRIVATE /W4)
else()
    target_compile_options(JSON_Parser PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Maintain folder structure in IDEs
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${MY_SOURCES})
