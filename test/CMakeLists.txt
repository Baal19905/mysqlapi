# The name of our project is "HELLO". CMakeLists files in this project can
# refer to the root src directory of the project as ${HELLO_src_DIR} and
# to the root binary directory of the project as ${HELLO_BINARY_DIR}.
cmake_minimum_required (VERSION 2.8)

#name.
project(mysqlapi)

SET(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -O0 -g3 -m64 -std=c++11 -fPIC -D__DEBUG__ -D__TEST_DEBUG__")	



#include dir.
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(/usr/include/mysql)



#source.
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/source SRC_FILES)

#target.
add_library(${PROJECT_NAME} STATIC ${SRC_FILES})

#include

install(
    TARGETS ${PROJECT_NAME}
    ARCHIVE DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/lib
)

