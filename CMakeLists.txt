cmake_minimum_required(VERSION 3.20)

project(NetRoute VERSION 0.1.0 LANGUAGES CXX)

add_executable(netroute
    src/main.cpp
)

set_target_properties(netroute PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(netroute PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
endif()