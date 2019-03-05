#cmake version
cmake_minimum_required (VERSION 3.11)

#set compiler
set (CMAKE_CXX_COMPILER "g++")

add_definitions(-std=c++11)

#project name
project (CONCURRENCY_SOCKET_PROJECT)

#find_package(Threads)
#target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})

#set compiler options
set (CMKAE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -lpthread")
message (STATUS "compiler_add_options -std=c++11 -lpthread")


#include directories
include_directories (${PROJECT_SOURCE_DIR}/common)

# libs directories
set (LIBRARY_OUT_PATH ${PROJECT_SOURCE_DIR}/libs)
link_directories (${LIBRARY_OUTPUT_PATH})

aux_source_directory (. DIR_SRCS)

#subdirectory
add_subdirectory (server)
add_subdirectory (client)
