cmake_minimum_required (VERSION 3.8.2)

add_executable (hello_world "hello_world.cpp")
target_link_libraries (hello_world PUBLIC injector)

add_executable (basic "basic.cpp")
target_link_libraries (basic PUBLIC injector)

add_executable (resolver "resolver.cpp")
target_link_libraries (resolver PUBLIC injector)

add_executable (compile_time "compile_time.cpp")
target_link_libraries (compile_time PUBLIC injector)
