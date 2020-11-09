cmake_minimum_required (VERSION 3.8.2)

add_executable (basic "basic.cpp")
target_link_libraries (basic PUBLIC injector)

add_executable (injection "injection.cpp")
target_link_libraries (injection PUBLIC injector)

add_executable (resolver "resolver.cpp")
target_link_libraries (resolver PUBLIC injector)

add_executable (invoke "invoke.cpp")
target_link_libraries (invoke PUBLIC injector)
