cmake_minimum_required (VERSION 3.8.2)

project (injector 
	VERSION 0.1.0
	DESCRIPTION "Simple dependency injector for C++"
	LANGUAGES CXX
)

option (INJECTOR_BUILD_EXAMPLES "Enable examples building" OFF)

add_library (injector INTERFACE)
target_include_directories (injector INTERFACE "include")
target_compile_features (injector INTERFACE cxx_std_17)

if (INJECTOR_BUILD_EXAMPLES)
	add_subdirectory ("examples")
endif ()
