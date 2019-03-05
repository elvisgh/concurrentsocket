aux_source_directory (. DIR_SRCS)

add_executable (server ${DIR_SRCS})

#include directories
include_directories (${PROJECT_SOURCE_DIR}/server/include)

include_directories (${PROJECT_SOURCE_DIR}/common)

find_package(Threads)
target_link_libraries(server ${CMAKE_THREAD_LIBS_INIT})

#target_link_libraries
