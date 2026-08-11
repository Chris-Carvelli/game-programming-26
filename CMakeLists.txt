cmake_minimum_required(VERSION 3.12)

project(gp25_exercises)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# 3rd party libraries
add_subdirectory(lib/SDL)

# repo's executables
add_subdirectory(examples)
add_subdirectory(playground)
