#include <stu/hashmap.hpp>
#include <unordered_set>
#include <gtest/gtest.h>
#include <iostream>
#include <fstream>
#include <chrono>

#include "ContainerTests.hpp"

class HashMapTests : public ContainerTests
{
protected:
	// Specific tests for HahsMap
	static constexpr std::string_view FunctionalInsertFile{"resources/dictInsertTest.txt"};
	static constexpr std::string_view dictSearchTestFile{"resources/dictSearchTest.txt"};
	static constexpr std::string_view dictRemoveTestFile{"resources/dictRemoveTest.txt"};
};

TEST_F(HashMapTests, DefaultConstructor)
{
	stu::hashmap map = stu::hashmap(1000);

	std::cout << "\n\n\n" << "Welcome to hashMap test!!! \n\n\n";
	/* EXPECT_EQ(tree.size(), 0);
	EXPECT_TRUE(tree.empty());*/
}

/*TEST_F(HashMapTests, mainHashFunctionCollisions)
{
	std::unordered_set<size_t> mainHashes;
	std::unordered_set<size_t> secHashes;
	mainHashes.reserve(1'000'000);
	secHashes.reserve(1'000'000);

	size_t collisionsMain = 0;
	size_t collisionsSec = 0;

	for (int i = 0; i < 1'000'000; i++)
	{
		size_t hashMain = stu::hashmap::hashMain(i);
		size_t hashSec = stu::hashmap::hashSecondary(i);

		if (mainHashes.contains(hashMain))
		{
			collisionsMain++;
		}
		else
		{
			mainHashes.insert(hashMain);
		}
		
		if (secHashes.contains(hashSec))
		{
			collisionsSec++;
		}
		else
		{
			secHashes.insert(hashSec);
		}
	}

	std::cout << "\n\n";
	std::cout << "Function HashMain finished hasing 1mil elements with " << collisionsMain << " collisions\n";
	std::cout << "Function HashSecondary finished hasing 1mil elements with " << collisionsSec << " collisions";
	std::cout << "\n\n";
}*/

TEST_F(HashMapTests, TestInsert)
{
	stu::hashmap map = stu::hashmap();

	map.insert(10, 100);
	map.insert(20, 200);
	map.insert(30, 300);

	ASSERT_TRUE(map.containsKey(10));
	ASSERT_TRUE(map.containsKey(20));
	ASSERT_TRUE(map.containsKey(30));
}

TEST_F(HashMapTests, TestInsertCollision)
{
	stu::hashmap map = stu::hashmap(10);

	// check for duplicates
	map.insert(10, 100);
	map.insert(10, 100);

	// check for collisons
	map.insert(20, 200);
	map.insert(30, 300);
	map.insert(30, 300);
	map.insert(40, 300);
	map.insert(50, 300);

	ASSERT_TRUE(map.containsKey(10));
	ASSERT_TRUE(map.containsKey(20));
	ASSERT_TRUE(map.containsKey(30));
	ASSERT_TRUE(map.containsKey(40));
	ASSERT_TRUE(map.containsKey(50));
}

TEST_F(HashMapTests, TestReserve)
{
	stu::hashmap map = stu::hashmap(10);

	map.insert(123, 0);
	map.insert(10, 100);
	map.insert(20, 200);
	map.insert(30, 300);
	map.insert(40, 300);
	map.insert(50, 500);
	map.insert(60, 600);
	map.insert(70, 700);
	

	ASSERT_EQ(map.capacity(), 29);
	ASSERT_TRUE(map.containsKey(10));
	ASSERT_TRUE(map.containsKey(20));
	ASSERT_TRUE(map.containsKey(30));
	ASSERT_TRUE(map.containsKey(40));
	ASSERT_TRUE(map.containsKey(50));
	ASSERT_TRUE(map.containsKey(60));
	ASSERT_TRUE(map.containsKey(70));
}

TEST_F(HashMapTests, FindIndexTest)
{
	stu::hashmap map = stu::hashmap(10);

	map.insert(10, 100);
	map.insert(20, 200);
	map.insert(30, 300);
	map.insert(40, 300);

	ASSERT_TRUE(map.findIndex(10) != -1);
	ASSERT_TRUE(map.findIndex(20) != -1);
	ASSERT_TRUE(map.findIndex(30) != -1);
	ASSERT_TRUE(map.findIndex(40) != -1);
	ASSERT_TRUE(map.findIndex(50) == -1);
}


TEST_F(HashMapTests, FindIndexTestCollision)
{
	stu::hashmap map = stu::hashmap(10);

	map.insert(2, 200);
	map.insert(60 , 300);

	ASSERT_TRUE(map.findIndex(2) != -1);
	ASSERT_TRUE(map.findIndex(60) != -1);
}


TEST_F(HashMapTests, RemoveTestNoCollision)
{
	stu::hashmap map = stu::hashmap(10);

	map.insert(10, 100);
	map.insert(20, 200);
	map.insert(30, 300);
	map.insert(40, 300);

	map.remove(10);
	map.remove(20);
	map.remove(30);
	map.remove(40);
}

std::vector<stu::hashmap::Entry> parseMap(std::ifstream& file, int reserve = 1000)
{
	std::vector<stu::hashmap::Entry> entries;
	entries.reserve(reserve);

	std::string line;
	int key;
	int value;
	char separator;

	while (std::getline(file, line))
	{
		std::istringstream iss(line);

		iss >> key >> separator >> value;

		entries.emplace_back(key, value, 0, false, false);
	}

	return entries;
}

void MapFileTest(std::string functionName, std::string_view fileName)
{
	std::cout << "\n" << functionName << "\n";

	std::ifstream file(fileName.data());

	ASSERT_TRUE(file.is_open());

	stu::hashmap map = stu::hashmap();
	std::string line;
	std::string command;
	int count = 0;

	std::chrono::nanoseconds insertTime{};
	std::chrono::nanoseconds searchTime{};
	std::chrono::nanoseconds removeTime{};

	// std::chrono::high_resolution_clock
	while (std::getline(file, line))
	{
		command.clear();
		count++;

		std::istringstream iss(line);
		int key;
		int value;
		char separator;
			
		iss >> command >> key;

		if (command == "INSERT" && iss >> separator >> value)
		{
			auto start = std::chrono::high_resolution_clock::now();

			map.insert(key, value);

			auto end = std::chrono::high_resolution_clock::now();
			insertTime += end - start;
		}
		else if (command == "DELETE")
		{
			auto start = std::chrono::high_resolution_clock::now();

			map.remove(key);

			auto end = std::chrono::high_resolution_clock::now();
			removeTime += end - start;
		}
		else if (command == "SEARCH" && iss >> separator >> value)
		{
			auto start = std::chrono::high_resolution_clock::now();

			int resultValue = map.search(key);

			auto end = std::chrono::high_resolution_clock::now();
			searchTime += end - start;

			ASSERT_EQ(value, resultValue);
		}
		else
		{
			break;
		}
	}

	std::vector<stu::hashmap::Entry> expected = parseMap(file);
	std::vector<stu::hashmap::Entry> actual = map.toVector();

	ASSERT_EQ(expected.size(), actual.size());

	std::cout << "InsertTime: " << std::chrono::duration_cast<std::chrono::milliseconds>(insertTime).count() << "\n";
	std::cout << "SearchTime: " << std::chrono::duration_cast<std::chrono::milliseconds>(searchTime).count() << "\n";
	std::cout << "RemoveTime: " << std::chrono::duration_cast<std::chrono::milliseconds>(removeTime).count() << "\n";
	std::cout << "\n";

	EXPECT_EQ(expected, actual);
}

TEST_F(HashMapTests, InsertFileTest)
{
	MapFileTest(__FUNCTION__, HashMapTests::FunctionalInsertFile);
}

TEST_F(HashMapTests, SearchFileTestAfterInsert)
{
	MapFileTest(__FUNCTION__, HashMapTests::dictSearchTestFile);
}

TEST_F(HashMapTests, RemoveTest)
{
	MapFileTest(__FUNCTION__, HashMapTests::dictRemoveTestFile);
}