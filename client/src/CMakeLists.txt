aux_source_directory (. DIR_SRCS)

add_executable (client ${DIR_SRCS})

#include directories
include_directories (${PROJECT_SOURCE_DIR}/client/include)

include_directories (${PROJECT_SOURCE_DIR}/common)

find_package(Threads)
target_link_libraries(client ${CMAKE_THREAD_LIBS_INIT})

#target_link_libraries
