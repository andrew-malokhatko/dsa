cmake_minimum_required(VERSION 3.16)

project(StuTests)

include(FetchContent)

# Download and include GoogleTest
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.16.0.zip
)
FetchContent_MakeAvailable(googletest)

add_executable(StuTests 
	./src/BTreeTests.cpp
    ./src/ContainerTests.hpp
    ./src/HashMapTests.cpp
	./src/main.cpp
)

target_link_libraries(StuTests PRIVATE stu gtest gtest_main)

# Add test
add_test(NAME StuTests COMMAND StuTests)

#add_custom_target(
#	TARGET StuTests ALL
#	POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E copy_directory
#    ${CMAKE_CURRENT_SOURCE_DIR}/resources  # Source directory
#    ${CMAKE_CURRENT_BINARY_DIR}/resources  # Destination directory
#)

# Copy resources folder to build directory
add_custom_target(CopyResources ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/resources  # Source directory
    ${CMAKE_CURRENT_BINARY_DIR}/resources  # Destination directory
)

add_dependencies(StuTests CopyResources)