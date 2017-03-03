cmake_minimum_required(VERSION 3.2)
project(sessiongate)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(thirdparty)

file(GLOB THIRDPARTY_SOURCE_FILES
        "thirdparty/*.h"
        "thirdparty/*.c"
        )

file(GLOB SOURCE_FILES
        "src/*.h"
        "src/*.cpp"
        )

if(STATIC_LINK)
    find_library(CRYPTOPP_LIBRARY NAMES libcryptopp.a PATHS "/usr/lib;/usr/local/lib")
    message("-- Linking to static Crypto++: ${CRYPTOPP_LIBRARY}")
    set(CMAKE_SHARED_LINKER_FLAGS "${CRYPTOPP_LIBRARY}")
else()
    set(CMAKE_SHARED_LINKER_FLAGS "-lcryptopp")
endif()

set(CMAKE_SHARED_LIBRARY_SUFFIX ".so")
add_library(sessiongate SHARED ${SOURCE_FILES} ${THIRDPARTY_SOURCE_FILES})
