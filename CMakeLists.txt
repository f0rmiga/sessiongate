cmake_minimum_required(VERSION 3.5)
project(session_gate)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(${CRYPTOPP_INCLUDE_DIR})
include_directories(thirdparty)

file(GLOB THIRDPARTY_SOURCE_FILES
        "thirdparty/*.h"
        "thirdparty/*.c"
        )

file(GLOB SOURCE_FILES
        "src/*.h"
        "src/*.cpp"
        )

set(CMAKE_SHARED_LINKER_FLAGS "-lcryptopp")

set(CMAKE_SHARED_LIBRARY_SUFFIX ".so")
add_library(session_gate SHARED ${SOURCE_FILES} ${THIRDPARTY_SOURCE_FILES})