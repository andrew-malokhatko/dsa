cmake_minimum_required(VERSION 3.16)

set (CMAKE_CXX_STANDARD 23)

project(BTree)

# Enable testing
enable_testing()

add_library(stu INTERFACE
	include/stu/btree.hpp
)

target_include_directories(stu INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(tests)