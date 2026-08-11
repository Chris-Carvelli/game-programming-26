file(GLOB file_src_list "*.c" "*.cpp")

add_executable(00_hello_world 00_hello_world.c)
add_executable(01_hello_world 01_hello_world.cpp)
add_executable(02_hello_sdl   02_hello_sdl.cpp)
target_link_libraries(02_hello_sdl PRIVATE SDL3::SDL3)
target_include_directories(02_hello_sdl PUBLIC lib/SDL/include)
