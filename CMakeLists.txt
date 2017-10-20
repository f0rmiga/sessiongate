cmake_minimum_required(VERSION 3.0)
project(SessionGate VERSION 0.0.1)

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0")

# source: https://cmake.org/pipermail/cmake/2007-May/014345.html
# KDE's with -fPIC
if(UNIX AND NOT WIN32)
  find_program(CMAKE_UNAME uname /bin /usr/bin /usr/local/bin )
  if(CMAKE_UNAME)
    exec_program(uname ARGS -m OUTPUT_VARIABLE CMAKE_SYSTEM_PROCESSOR)
    set(CMAKE_SYSTEM_PROCESSOR ${CMAKE_SYSTEM_PROCESSOR} CACHE INTERNAL "processor type (i386 and x86_64)")
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
      add_definitions(-fPIC)
    endif(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
  endif(CMAKE_UNAME)
endif(UNIX AND NOT WIN32)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

# This helps https://github.com/vector-of-bool/vscode-cmake-tools
include(CMakeToolsHelpers OPTIONAL)

if(NOT DEFINED LIBSODIUM_DIR)
  find_path(
    LIBSODIUM_DIR
    NAMES libsodium
    PATHS "/usr/include" "/usr/local/include" "/usr/local/opt"
    NO_DEFAULT_PATH
  )
endif(NOT DEFINED LIBSODIUM_DIR)
message("-- Using LIBSODIUM include dir: ${LIBSODIUM_DIR}/libsodium/include")

set(RMSDK_DIR "${PROJECT_SOURCE_DIR}/deps/RedisModulesSDK")
set(RMUTIL_DIR "${RMSDK_DIR}/rmutil")

include_directories("${PROJECT_SOURCE_DIR}/src" "${RMSDK_DIR}" "${RMUTIL_DIR}" "${LIBSODIUM_DIR}/libsodium/include")
set(CMAKE_SHARED_LIBRARY_SUFFIX ".so")
add_library(sessiongate SHARED
  src/module.c
  src/utils.c
  src/start.c
  src/end.c
  src/pset.c
  src/pget.c
  src/pdel.c
  src/expire.c
  ${RMUTIL_DIR}/util.c)
set_target_properties(sessiongate PROPERTIES PREFIX "" C_VISIBILITY_PRESET hidden LINK_FLAGS "-Bsymbolic")
set_target_properties(sessiongate PROPERTIES COMPILE_FLAGS "-fPIC")
target_compile_definitions(sessiongate PUBLIC REDIS_MODULE_TARGET)

if(NOT DEFINED LIBSODIUM_LIB_DIR)
  find_library(LIBSODIUM_LIB_DIR NAMES libsodium.a PATHS "/usr/lib" "/usr/local/lib" "/usr/local/opt/libsodium/lib/")
endif(NOT DEFINED LIBSODIUM_LIB_DIR)
message("-- Linking to static LIBSODIUM libsodium.a: ${LIBSODIUM_LIB_DIR}")
target_link_libraries(sessiongate PRIVATE ${LIBSODIUM_LIB_DIR})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

include(CPack)
include_directories()
