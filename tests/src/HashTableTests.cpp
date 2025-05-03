//#include <chrono>
//#include <fstream>
//#include <gtest/gtest.h>
//#include <iostream>
//#include <unordered_set>
//#include <algorithm>
//#include <list>
//
//#include "ContainerTests.hpp"
//#include "stu/hashtable.hpp"
//
//
//class HashTableTests : public ContainerTests
//{
//protected:
//
//	using key_type = int;
//	using value_type = int;
//
//	// Specific tests for HahsMap
//	static constexpr std::string_view FunctionalInsertFile{"resources/dictInsertTest.txt"};
//	static constexpr std::string_view dictSearchTestFile{"resources/dictSearchTest.txt"};
//	static constexpr std::string_view dictRemoveTestFile{"resources/dictRemoveTest.txt"};
//};
//
//TEST_F(HashTableTests, Default)
//{
//	std::cout << "\n\n\n" << "Welcome to HashTable test!!! \n\n\n";
//}
//
//TEST_F(HashTableTests, TestInsert)
//{
//	stu::hashtable<int, int> table(10);
//
//	table.insert(10, 100);
//	table.insert(20, 200);
//	table.insert(30, 300);
//
//	ASSERT_TRUE(table.contains(10));
//	ASSERT_TRUE(table.contains(20));
//	ASSERT_TRUE(table.contains(30));
//}
//
///*
//TEST_F(HashTableTests, TestInsertCollision)
//{
//	stu::hashtable<int, int> table(10);
//
//	// check for duplicates
//	table.insert(10, 100);
//	table.insert(10, 100);
//
//	// check for collisons
//	table.insert(20, 200);
//	table.insert(30, 300);
//	table.insert(30, 300);
//	table.insert(40, 300);
//	table.insert(50, 300);
//
//	ASSERT_TRUE(table.contains(10));
//	ASSERT_TRUE(table.contains(20));
//	ASSERT_TRUE(table.contains(30));
//	ASSERT_TRUE(table.contains(40));
//	ASSERT_TRUE(table.contains(50));
//}
//
//TEST_F(HashTableTests, TestReserve)
//{
//	stu::hashtable<int, int> map (10);
//
//	map.insert(123, 0);
//	map.insert(10, 100);
//	map.insert(20, 200);
//	map.insert(30, 300);
//	map.insert(40, 300);
//	map.insert(50, 500);
//	map.insert(60, 600);
//	map.insert(70, 700);
//
//	ASSERT_TRUE(map.contains(10));
//	ASSERT_TRUE(map.contains(20));
//	ASSERT_TRUE(map.contains(30));
//	ASSERT_TRUE(map.contains(40));
//	ASSERT_TRUE(map.contains(50));
//	ASSERT_TRUE(map.contains(60));
//	ASSERT_TRUE(map.contains(70));
//}
//
//TEST_F(HashTableTests, FindIndexTest)
//{
//	stu::hashtable<int, int> map (10);
//
//	map.insert(10, 100);
//	map.insert(20, 200);
//	map.insert(30, 300);
//	map.insert(40, 300);
//
//	ASSERT_TRUE(map.contains(10));
//	ASSERT_TRUE(map.contains(20));
//	ASSERT_TRUE(map.contains(30));
//	ASSERT_TRUE(map.contains(40));
//	ASSERT_FALSE(map.contains(50));
//}
//
//
//TEST_F(HashTableTests, FindIndexTestCollision)
//{
//	stu::hashtable<int, int> map;
//
//	map.insert(2, 200);
//	map.insert(60, 300);
//
//	ASSERT_TRUE(map.contains(2));
//	ASSERT_TRUE(map.contains(60));
//}
//
//
//TEST_F(HashTableTests, RemoveTestNoCollision)
//{
//	stu::hashtable<int, int> map;
//
//	map.insert(10, 100);
//	map.insert(20, 200);
//	map.insert(30, 300);
//	map.insert(40, 300);
//
//	map.remove(10);
//	map.remove(20);
//	map.remove(30);
//	map.remove(40);
//}
//
//std::vector<stu::hashtable<int, int>::Node> parseFileMap(std::ifstream& file, int reserve = 1000)
//{
//	std::vector<stu::hashtable<int, int>::Node> nodes;
//	nodes.reserve(reserve);
//
//	std::string line;
//	int key;
//	int value;
//	char separator;
//
//	while (std::getline(file, line))
//	{
//		std::istringstream iss(line);
//
//		iss >> key >> separator >> value;
//
//		nodes.emplace_back(key, value);
//	}
//
//	return nodes;
//}
//
//		// Returns a vector sorted by keys
//std::vector<stu::hashtable<int, int>::Node> parseTable(const stu::hashtable<int, int>& table)
//{
//	using Bucket = stu::hashtable<int, int>::Bucket;
//	using Node = stu::hashtable<int, int>::Node;
//
//	std::vector<Node> result;
//	result.reserve(table.getCount());
//	Bucket* buckets = table.getBuckets();
//
//	for (size_t i = 0; i < table.getCapacity(); ++i)
//	{
//		Bucket bucket = buckets[i];
//		Node* current = bucket.root;
//
//		while (current)
//		{
//			result.push_back(*current);
//
//			current = current->next;
//		}
//	}
//
//	std::sort(result.begin(),result.end(),
//		[](const Node& a, const Node& b)
//		{
//			return a.m_key < b.m_key;
//		});
//
//	return result;
//}
//
//void HashTableFileTest(std::string functionName, std::string_view fileName)
//{
//	using Bucket = stu::hashtable<int, int>::Bucket;
//	using Node = stu::hashtable<int, int>::Node;
//
//	std::cout << "\n" << functionName << "\n";
//
//	std::ifstream file(fileName.data());
//
//	ASSERT_TRUE(file.is_open());
//
//	stu::hashtable<int, int> table;
//	std::string line;
//	std::string command;
//	int count = 0;
//
//	std::chrono::nanoseconds insertTime{};
//	std::chrono::nanoseconds searchTime{};
//	std::chrono::nanoseconds removeTime{};
//
//	// std::chrono::high_resolution_clock
//	while (std::getline(file, line))
//	{
//		command.clear();
//		count++;
//
//		std::istringstream iss(line);
//		int key;
//		int value;
//		char separator;
//
//		iss >> command >> key;
//
//		if (command == "INSERT" && iss >> separator >> value)
//		{
//			auto start = std::chrono::high_resolution_clock::now();
//
//			table.insert(key, value);
//
//			auto end = std::chrono::high_resolution_clock::now();
//			insertTime += end - start;
//		}
//		else if (command == "DELETE")
//		{
//			auto start = std::chrono::high_resolution_clock::now();
//
//			table.remove(key);
//
//			auto end = std::chrono::high_resolution_clock::now();
//			removeTime += end - start;
//		}
//		else if (command == "SEARCH" && iss >> separator >> value)
//		{
//			auto start = std::chrono::high_resolution_clock::now();
//
//			int resultValue = table.search(key);
//
//			auto end = std::chrono::high_resolution_clock::now();
//			searchTime += end - start;
//
//			ASSERT_EQ(value, resultValue);
//		}
//		else
//		{
//			break;
//		}
//	}
//
//	std::vector<Node> expected = parseFileMap(file);
//	std::vector<Node> actual = parseTable(table);
//
//	ASSERT_EQ(expected.size(), actual.size());
//
//	std::cout << "InsertTime: " << std::chrono::duration_cast<std::chrono::milliseconds>(insertTime).count() << "\n";
//	std::cout << "SearchTime: " << std::chrono::duration_cast<std::chrono::milliseconds>(searchTime).count() << "\n";
//	std::cout << "RemoveTime: " << std::chrono::duration_cast<std::chrono::milliseconds>(removeTime).count() << "\n";
//	std::cout << "\n";
//
//	table.~table();
//
//	EXPECT_EQ(expected, actual);
//}
//
////TEST_F(HashTableTests, InsertFileTest)
////{
////	HashTableFileTest(__FUNCTION__, HashTableTests::FunctionalInsertFile);
////}
////
////TEST_F(HashTableTests, RemoveTest)
////{
////	HashTableFileTest(__FUNCTION__, HashTableTests::dictRemoveTestFile);
////}
//*/