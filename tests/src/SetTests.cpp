#include <gtest/gtest.h>
#include <array>
#include <algorithm>

#include "ContainerTests.hpp"
#include "stu/set.hpp"

class SetTests : public ContainerTests
{
protected:
	stu::set<int> testSet{};
};

TEST_F(SetTests, Default)
{
	std::cout << "\n\n\n" << "Welcome to Set test!!! \n\n\n";
}


TEST_F(SetTests, InsertIntoEmptySet)
{
	EXPECT_TRUE(testSet.insert(10));
	EXPECT_TRUE(testSet.contains(10));
}

TEST_F(SetTests, InsertDuplicateValue)
{
	EXPECT_TRUE(testSet.insert(20));
	EXPECT_FALSE(testSet.insert(20)); // Duplicate insertion should fail
}

TEST_F(SetTests, RemoveExistingValue)
{
	testSet.insert(30);
	EXPECT_TRUE(testSet.remove(30));
	EXPECT_FALSE(testSet.contains(30));
}

TEST_F(SetTests, RemoveNonExistingValue)
{
	EXPECT_FALSE(testSet.remove(40)); // Removing a non-existent value
}

TEST_F(SetTests, ContainsValue)
{
	testSet.insert(50);
	EXPECT_TRUE(testSet.contains(50));
	EXPECT_FALSE(testSet.contains(60)); // Value not in the set
}

TEST_F(SetTests, InsertMultipleValues)
{
	EXPECT_TRUE(testSet.insert(70));
	EXPECT_TRUE(testSet.insert(80));
	EXPECT_TRUE(testSet.insert(90));
	EXPECT_TRUE(testSet.contains(70));
	EXPECT_TRUE(testSet.contains(80));
	EXPECT_TRUE(testSet.contains(90));
}

TEST_F(SetTests, RemoveRootNode)
{
	testSet.insert(100);
	EXPECT_TRUE(testSet.remove(100)); // Removing the root node
	EXPECT_FALSE(testSet.contains(100));
}

TEST_F(SetTests, RemoveLeafNode)
{
	testSet.insert(110);
	testSet.insert(120);
	EXPECT_TRUE(testSet.remove(120)); // Removing a leaf node
	EXPECT_FALSE(testSet.contains(120));
}

TEST_F(SetTests, RemoveNodeWithOneChild)
{
	testSet.insert(130);
	testSet.insert(140);
	testSet.insert(135); // Node with one child
	EXPECT_TRUE(testSet.remove(140));
	EXPECT_FALSE(testSet.contains(140));
	EXPECT_TRUE(testSet.contains(135));
}

TEST_F(SetTests, RemoveNodeWithTwoChildren)
{
	testSet.insert(150);
	testSet.insert(140);
	testSet.insert(160);
	EXPECT_TRUE(testSet.remove(150)); // Node with two children
	EXPECT_FALSE(testSet.contains(150));
	EXPECT_TRUE(testSet.contains(140));
	EXPECT_TRUE(testSet.contains(160));
}

TEST_F(SetTests, RemoveNodeWithTwoChildrenAndSuccessorHasChildren)
{
    testSet.insert(200);
    testSet.insert(150);
    testSet.insert(250);
    testSet.insert(240); // Successor of 200
    testSet.insert(245); // Child of successor

    EXPECT_TRUE(testSet.remove(200)); // Removing node with two children
    EXPECT_FALSE(testSet.contains(200));
    EXPECT_TRUE(testSet.contains(150));
    EXPECT_TRUE(testSet.contains(250));
    EXPECT_TRUE(testSet.contains(240));
    EXPECT_TRUE(testSet.contains(245));
}

TEST_F(SetTests, RemoveNodeWithTwoChildrenAndPredecessorHasChildren) {
    testSet.insert(300);
    testSet.insert(250);
    testSet.insert(350);
    testSet.insert(240); // Predecessor of 300
    testSet.insert(245); // Child of predecessor

    EXPECT_TRUE(testSet.remove(300)); // Removing node with two children
    EXPECT_FALSE(testSet.contains(300));
    EXPECT_TRUE(testSet.contains(250));
    EXPECT_TRUE(testSet.contains(350));
    EXPECT_TRUE(testSet.contains(240));
    EXPECT_TRUE(testSet.contains(245));
}

TEST_F(SetTests, RemoveNodesInSequence) {
    testSet.insert(400);
    testSet.insert(300);
    testSet.insert(500);
    testSet.insert(250);
    testSet.insert(350);
    testSet.insert(450);
    testSet.insert(550);

    EXPECT_TRUE(testSet.remove(400)); // Remove root
    EXPECT_TRUE(testSet.remove(300)); // Remove left child
    EXPECT_TRUE(testSet.remove(500)); // Remove right child
    EXPECT_TRUE(testSet.remove(250)); // Remove leaf
    EXPECT_TRUE(testSet.remove(350)); // Remove leaf
    EXPECT_TRUE(testSet.remove(450)); // Remove leaf
    EXPECT_TRUE(testSet.remove(550)); // Remove leaf

    EXPECT_FALSE(testSet.contains(400));
    EXPECT_FALSE(testSet.contains(300));
    EXPECT_FALSE(testSet.contains(500));
    EXPECT_FALSE(testSet.contains(250));
    EXPECT_FALSE(testSet.contains(350));
    EXPECT_FALSE(testSet.contains(450));
    EXPECT_FALSE(testSet.contains(550));
}

TEST_F(SetTests, Iteration)
{
	std::array testValues = {400, 300, 500, 250, 350, 450, 550};

	for (auto v : testValues) 
	{
		testSet.insert(v);
	}

	std::sort(testValues.begin(), testValues.end());
	size_t index = 0;

	for (const auto& v : testSet) 
	{
		ASSERT_EQ(v, testValues[index++]);
	}

	return;
}

TEST_F(SetTests, Comparator)
{
	std::array testValues = {400, 300, 500, 250, 350, 450, 550};

	stu::set<int, std::greater<int>> testSetGreater{};

	for (auto v : testValues)
	{
		testSetGreater.insert(v);
	}

	std::sort(testValues.begin(), testValues.end(), std::greater<int>());
	size_t index = 0;

	for (const auto& v : testSetGreater)
	{
		ASSERT_EQ(v, testValues[index++]);
	}

	return;
}

TEST_F(SetTests, CopyConstructor)
{
	std::array testValues = {400, 300, 500, 250, 350, 450, 550};

	for (auto v : testValues)
	{
		testSet.insert(v);
	}

	stu::set<int> copy = testSet;

	// Different addresses
	ASSERT_NE(copy.root(), testSet.root());
	ASSERT_NE(copy.root()->left, testSet.root()->left);
	ASSERT_NE(copy.root()->right, testSet.root()->right);

	// Same Values
	ASSERT_EQ(copy.root()->value, testSet.root()->value);
	ASSERT_EQ(copy.root()->left->value, testSet.root()->left->value);
	ASSERT_EQ(copy.root()->right->value, testSet.root()->right->value);
}

TEST_F(SetTests, MoveConstructor)
{
	std::array testValues = {400, 300, 500, 250, 350, 450, 550};

	for (auto v : testValues)
	{
		testSet.insert(v);
	}

	stu::set<int> moved = std::move(testSet);

	ASSERT_EQ(testSet.root(), nullptr);
	ASSERT_EQ(testSet.size(), 0);

	ASSERT_EQ(moved.size(), 7);

	for (const auto& val : testValues)
	{
		ASSERT_TRUE(moved.contains(val));
	}
}

TEST_F(SetTests, MoveAssignmentOperator)
{
	std::array testValues = {400, 300, 500, 250, 350, 450, 550};
	std::array testValues2 = {1, 2, 3};

	for (auto v : testValues)
	{
		testSet.insert(v);
	}

	stu::set<int> movedLater;

	for (auto v : testValues2)
	{
		movedLater.insert(v);
	}

	// Move values from testSet to movedLater
	movedLater = std::move(testSet);

	ASSERT_EQ(testSet.root(), nullptr);
	ASSERT_EQ(testSet.size(), 0);

	ASSERT_EQ(movedLater.size(), 7);
	
	for (const auto& val : testValues)
	{
		ASSERT_TRUE(movedLater.contains(val));
	}

	for (const auto& val : testValues2)
	{
		ASSERT_FALSE(movedLater.contains(val));
	}
}

TEST_F(SetTests, CopyAssignmentOperator)
{
	std::array testValues = {400, 300, 500, 250, 350, 450, 550};
	std::array testValues2 = {1, 2, 3};

	for (auto v : testValues)
	{
		testSet.insert(v);
	}

	stu::set<int> copiedLater;

	for (auto v : testValues2)
	{
		copiedLater.insert(v);
	}

	copiedLater = testSet;

	ASSERT_EQ(copiedLater.size(), 7);
	ASSERT_EQ(testSet.size(), 7);

	// Different addresses
	ASSERT_NE(copiedLater.root(), testSet.root());
	ASSERT_NE(copiedLater.root()->left, testSet.root()->left);
	ASSERT_NE(copiedLater.root()->right, testSet.root()->right);

	// Same Values
	ASSERT_EQ(copiedLater.root()->value, testSet.root()->value);
	ASSERT_EQ(copiedLater.root()->left->value, testSet.root()->left->value);
	ASSERT_EQ(copiedLater.root()->right->value, testSet.root()->right->value);
}
