# The name of our project is "HELLO". CMakeLists files in this project can
# refer to the root src directory of the project as ${HELLO_src_DIR} and
# to the root binary directory of the project as ${HELLO_BINARY_DIR}.
cmake_minimum_required (VERSION 2.8)

#name.
project(tbl_generator)
set(PRJ_VER "1.0.2")


#include compile macro.
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O2 -std=c++11")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -g -std=c++11")
set(CMAKE_SKIP_RPATH TRUE)



#define
set(MYSQL_CLIENT ${CMAKE_CURRENT_SOURCE_DIR}/../src)


#include dir.
include_directories(/usr/include/mysql/)
include_directories(${MYSQL_CLIENT})

#link dir
link_directories(/usr/lib64/mysql/)

#source.
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRC_FILES)
aux_source_directory(${MYSQL_CLIENT} SRC_FILES)

#target.
add_executable(${PROJECT_NAME} ${SRC_FILES})
target_link_libraries (${PROJECT_NAME} mysqlclient)
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PRJ_VER})


#include
install (TARGETS   ${PROJECT_NAME} DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin )

