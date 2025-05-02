#include <algorithm>
#include <chrono>
#include <fstream>
#include <gtest/gtest.h>
#include <iostream>
#include <list>
#include <unordered_set>

#include "ContainerTests.hpp"
#include "stu/list.hpp"


class ListTests : public ContainerTests
{
protected:
	using key_type = int;
	using value_type = int;

	// Specific tests for HahsMap
	static constexpr std::string_view FunctionalInsertFile{"resources/dictInsertTest.txt"};
	static constexpr std::string_view dictSearchTestFile{"resources/dictSearchTest.txt"};
	static constexpr std::string_view dictRemoveTestFile{"resources/dictRemoveTest.txt"};
};

TEST_F(ListTests, Default)
{
	std::cout << "\n\n\n" << "Welcome to HashTable test!!! \n\n\n";
}

TEST_F(ListTests, TestInsert)
{
	stu::list<int> l;

	l.insert(10);
	l.insert(20);
	l.insert(30);
	l.insert(40);

	for (auto v : l) 
	{
		std::cout << v;
	}
}