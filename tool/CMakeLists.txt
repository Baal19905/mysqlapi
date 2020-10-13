# The name of our project is "HELLO". CMakeLists files in this project can
# refer to the root src directory of the project as ${HELLO_src_DIR} and
# to the root binary directory of the project as ${HELLO_BINARY_DIR}.
cmake_minimum_required (VERSION 2.8)

#name.
project(tbl_generator)
set(PRJ_VER "1.0.1")


#include compile macro.
include(${CMAKE_CURRENT_SOURCE_DIR}/../../head.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/../head.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/../dev_version.cmake)


#define
set(MYSQL_CLIENT ${CMAKE_CURRENT_SOURCE_DIR}/../mysql_client)
set(MYSQL ${CMAKE_CURRENT_SOURCE_DIR}/../../../third_dependency/mysql/)


#include dir.
include_directories(${MYSQL_CLIENT}/include)
include_directories(${MYSQL}/mysql)

#link dir
link_directories(${MYSQL}/lib)

#source.
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRC_FILES)
aux_source_directory(${MYSQL_CLIENT}/source SRC_FILES)

#target.
add_executable(${PROJECT_NAME} ${SRC_FILES})
target_link_libraries (${PROJECT_NAME} mysqlclient)
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PRJ_VER}${_SVN_VER})


#include
install (TARGETS   ${PROJECT_NAME} DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin )

