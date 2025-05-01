#pragma once

#include <gtest/gtest.h>
#include <string_view>

class ContainerTests : public testing::Test
{
protected:

	// General tests for appr. 1000 elements
	static constexpr std::string_view insertFilePath{"resources/insertTest.txt"};
	static constexpr std::string_view searchFilePath{"resources/searchTest.txt"};
	static constexpr std::string_view removeFilePath{"resources/removeTest.txt"};

	// Performance tests appr 1mil elements
	static constexpr std::string_view perfInsertFilePath{"resources/perfInsertTest.txt"};
	static constexpr std::string_view perfSearchFilePath{"resources/perfSearchTest.txt"};
};