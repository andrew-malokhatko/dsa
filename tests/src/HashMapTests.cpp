#include <stu/hashmap.hpp>
#include <gtest/gtest.h>

#include "ContainerTests.hpp"

class HashMapTests : public ContainerTests
{
protected:
	// Specific tests for HahsMap
};

TEST_F(HashMapTests, DefaultConstructor)
{
	stu::hashmap map = stu::hashmap(1000);

	std::cout << "\n\n\n" << "Welcome to hashMap test!!! \n\n\n";
	/* EXPECT_EQ(tree.size(), 0);
	EXPECT_TRUE(tree.empty());*/
}

