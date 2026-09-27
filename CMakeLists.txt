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

find_path(PCAP_INCLUDE_DIR NAMES pcap/pcap.h REQUIRED)
find_library(PCAP_LIBRARY NAMES pcap REQUIRED)

target_include_directories(netroute PRIVATE "${PCAP_INCLUDE_DIR}")
target_link_libraries(netroute PRIVATE "${PCAP_LIBRARY}")