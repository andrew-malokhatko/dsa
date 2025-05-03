cmake_minimum_required(VERSION 3.16)

set (CMAKE_CXX_STANDARD 23)

project(BTree)

# Enable testing
include(CTest)
enable_testing()

add_library(stu INTERFACE
	include/stu/btree.hpp
	include/stu/hashmap.hpp
	include/stu/hashtable.hpp
	include/stu/list.hpp
	include/stu/bdd.hpp
	include/stu/bdd2.hpp
	include/stu/utils.hpp
)

target_include_directories(stu INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(tests)