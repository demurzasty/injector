cmake_minimum_required (VERSION 3.8.2)

add_executable (hello_world "hello_world.cpp")
target_link_libraries (hello_world PUBLIC injector)
