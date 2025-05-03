cmake_minimum_required(VERSION 3.16)

project(StuTests)

include(FetchContent)

# Download and include GoogleTest
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.16.0.zip
)
FetchContent_MakeAvailable(googletest)

add_executable( StuTests 
	./src/BTreeTests.cpp
    ./src/ContainerTests.hpp
    ./src/HashMapTests.cpp
	./src/main.cpp
    ./src/BDDTests.cpp
    ./src/BDD2Tests.cpp
    ./src/TestUtils.cpp
    ./src/HashTableTests.cpp
    ./src/ListTests.cpp
    ./src/SetTests.cpp)

target_link_libraries(StuTests PRIVATE stu gtest gtest_main)

# Add test
add_test(NAME StuTests COMMAND StuTests)

# Copy resources folder to build directory
add_custom_target(CopyResources ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/resources  # Source directory
    ${CMAKE_CURRENT_BINARY_DIR}/resources  # Destination directory
)

# Copy baty files for testing folder to build directory
add_custom_target(CopyGenerators ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/generator  # Source directory
    ${CMAKE_CURRENT_BINARY_DIR}/generator  # Destination directory
)

# Copy baty files for testing folder to build directory
add_custom_target(CopyBatFiles ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/testers     # Source directory
    ${CMAKE_CURRENT_BINARY_DIR}             # Destination directory
)

add_dependencies(StuTests CopyResources)
add_dependencies(StuTests CopyGenerators)
add_dependencies(StuTests CopyBatFiles)