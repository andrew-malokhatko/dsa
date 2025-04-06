#include <stu/btree.hpp>
#include <gtest/gtest.h>
#include <iostream>
#include <fstream>
#include <chrono>

#include "ContainerTests.hpp"


class BTreeTests : public ContainerTests
{
  protected:
	// Specific test cases for BTree
	static constexpr std::string_view FunctionalRemove100FilePath{"resources/funcRemoveTest100.txt"};
	static constexpr std::string_view FunctionalRemove50FilePath{"resources/funcRemoveTest50.txt"};
	static constexpr std::string_view FunctionalRemoveNULLFilePath{"resources/funcRemoveTestNULL.txt"};
	static constexpr std::string_view funcRemoveFindPre{"resources/funcRemoveFindPre.txt"};
};

TEST_F(BTreeTests, DefaultConstructor)
{
    stu::btree tree;

    EXPECT_EQ(tree.size(), 0);
    EXPECT_TRUE(tree.empty());
}

TEST_F(BTreeTests, InsertIntoEmptyTree)
{
    stu::btree tree;

    tree.insert(100);
    EXPECT_EQ(tree.size(), 1);
    EXPECT_FALSE(tree.empty());
}

TEST_F(BTreeTests, InsertMultipleKeys)
{
    stu::btree tree;

    tree.insert(100);
    tree.insert(50);
    tree.insert(150);
    EXPECT_EQ(tree.size(), 3);
}

TEST_F(BTreeTests, InsertAndBalance)
{
    stu::btree tree;

    tree.insert(100);
    tree.insert(50);
    tree.insert(150);
    tree.insert(25);
    tree.insert(75);
    tree.insert(125);
    tree.insert(175);
    EXPECT_EQ(tree.size(), 7);
}

TEST_F(BTreeTests, BNodeIsLeaf)
{
    stu::btree::BNode node;
    EXPECT_TRUE(node.isLeaf());
    node.childrenSize = 1;
    EXPECT_FALSE(node.isLeaf());
}

TEST_F(BTreeTests, BNodeIsRoot)
{
    stu::btree::BNode node;
    EXPECT_TRUE(node.isRoot());
    node.parent = new stu::btree::BNode();
    EXPECT_FALSE(node.isRoot());
    delete node.parent;
}

TEST_F(BTreeTests, BNodeHasParent)
{
    stu::btree::BNode node;
    EXPECT_FALSE(node.hasParent());
    node.parent = new stu::btree::BNode();
    EXPECT_TRUE(node.hasParent());
    delete node.parent;
}

#if 0
TEST_F(BTreeTests, BNodeIsBalanced)
{
    stu::btree::BNode node;
    EXPECT_TRUE(node.isBalanced());
    
    node.size = 1;
    node.childrenSize = 1; // It is not a parent anymore
    EXPECT_TRUE(node.isBalanced());
    node.size = 4;
    EXPECT_FALSE(node.isBalanced());
}
#endif

TEST_F(BTreeTests, BNodeFindInsertPos)
{
    stu::btree::BNode node;
    node.keys(0) = 50;
    node.keys(1) = 100;
    node.size = 2;
    EXPECT_EQ(node.findInsertPos(75), 1);
    EXPECT_EQ(node.findInsertPos(150), 2);
}

TEST_F(BTreeTests, BNodeFindIndex)
{
    stu::btree::BNode node;
    node.keys(0) = 50;
    node.keys(1) = 100;
    node.size = 2;
    EXPECT_EQ(node.findIndex(50), 0);
    EXPECT_EQ(node.findIndex(100), 1);
    EXPECT_EQ(node.findIndex(75), -1);
}

TEST_F(BTreeTests, BNodeInsertKey)
{
    stu::btree::BNode node;
    node.insertKey(50);
    node.insertKey(100);
    EXPECT_EQ(node.size, 2);
    EXPECT_EQ(node.keys(0), 50);
    EXPECT_EQ(node.keys(1), 100);
}

TEST_F(BTreeTests, BNodeRemoveKey)
{
    stu::btree::BNode node;
    node.insertKey(50);
    node.insertKey(100);
    node.removeKey(50);
    EXPECT_EQ(node.size, 1);
    EXPECT_EQ(node.keys(0), 100);

    node.removeKey(100);
    EXPECT_EQ(node.size, 0);

    node.removeKey(100);
    EXPECT_EQ(node.size, 0);
}

TEST_F(BTreeTests, BNodeInsertChild)
{
    stu::btree::BNode node;
    stu::btree::BNode child;
    node.insertChild(&child);
    EXPECT_EQ(node.childrenSize, 1);
    EXPECT_EQ(node.children(0), &child);
}

TEST_F(BTreeTests, BNodeRemoveChild)
{
    stu::btree::BNode node;
    stu::btree::BNode child;
    node.insertChild(&child);
    node.removeChild(&child);
    EXPECT_EQ(node.childrenSize, 0);
}

TEST_F(BTreeTests, BalanceInsert)
{
    stu::btree tree;
    stu::btree::BNode node;
    node.insertKey(50);
    node.insertKey(100);
    node.insertKey(150);
    node.insertKey(250);
    tree.balanceInsert(&node);
    EXPECT_EQ(node.size, 2); // After balancing, the node should be split
}

TEST_F(BTreeTests, SplitNode)
{
    stu::btree tree;
    stu::btree::BNode node;
    node.insertKey(50);
    node.insertKey(100);
    node.insertKey(150);
    tree.split(&node, 1);
    EXPECT_EQ(node.size, 1); // After splitting, the node should have 2 keys
}

TEST_F(BTreeTests, CheckFiles)
{
    std::ifstream insertFile(BTreeTests::insertFilePath.data());
    std::ifstream perfInsertFile(BTreeTests::perfInsertFilePath.data());

    ASSERT_TRUE(insertFile.is_open());
    ASSERT_TRUE(perfInsertFile.is_open());
}

std::vector<int> parseArray(std::string& line, int reserve = 1000)
{
    std::vector<int> numbers;
    numbers.reserve(reserve);

    std::istringstream ss(line);

    char ch;
    int num;

    ss >> ch;

    while (ss >> num)
    {
        numbers.push_back(num);
        ss >> ch;
    }

    return numbers;
}

TEST_F(BTreeTests, getChildrenTest)
{
    std::vector<int> expected{ 100, 200, 300, 400, 500, 600, 700 };
    std::vector<int> actual;

    stu::btree::BNode root;
    stu::btree::BNode child1;
    stu::btree::BNode child2;
    stu::btree::BNode child3;
    stu::btree::BNode child4;

    child1.insertKey(100);
    child2.insertKey(300);
    child3.insertKey(500);
    child4.insertKey(700);

    root.insertKey(200);
    root.insertKey(400);
    root.insertKey(600);

    root.insertChild(&child1);
    root.insertChild(&child2);
    root.insertChild(&child3);
    root.insertChild(&child4);

    root.getChildren(actual);

    ASSERT_EQ(expected, actual);
}

TEST_F(BTreeTests, RightSiblingTest)
{
    stu::btree tree;
    stu::btree::BNode root;
    stu::btree::BNode child1;
    stu::btree::BNode child2;
    stu::btree::BNode child3;

    child1.insertKey(50);
    child2.insertKey(100);
    child3.insertKey(150);

    root.insertKey(75);
    root.insertKey(125);

    root.insertChild(&child1);
    root.insertChild(&child2);
    root.insertChild(&child3);

    EXPECT_EQ(child1.rightSibling(), &child2);
    EXPECT_EQ(child2.rightSibling(), &child3);
    EXPECT_EQ(child3.rightSibling(), nullptr);
}

TEST_F(BTreeTests, LeftSiblingTest)
{
    stu::btree::BNode root;
    stu::btree::BNode child1;
    stu::btree::BNode child2;
    stu::btree::BNode child3;

    child1.insertKey(50);
    child2.insertKey(100);
    child3.insertKey(150);

    root.insertKey(75);
    root.insertKey(125);

    root.insertChild(&child1);
    root.insertChild(&child2);
    root.insertChild(&child3);

    EXPECT_EQ(child1.leftSibling(), nullptr);
    EXPECT_EQ(child2.leftSibling(), &child1);
    EXPECT_EQ(child3.leftSibling(), &child2);
}

TEST_F(BTreeTests, BorrowKeyLeftTest)
{
    stu::btree::BNode root;
    stu::btree::BNode leftChild;
    stu::btree::BNode rightChild;

    leftChild.insertKey(50);
    leftChild.insertKey(60);
    leftChild.insertKey(70);

    rightChild.insertKey(100);

    root.insertKey(80);
    root.insertChild(&leftChild);
    root.insertChild(&rightChild);

    EXPECT_EQ(leftChild.size, 3);
    EXPECT_EQ(rightChild.size, 1);

    rightChild.borrowLeft(&rightChild, &root, &leftChild);

    EXPECT_EQ(leftChild.size, 2);
    EXPECT_EQ(rightChild.size, 2);
    EXPECT_EQ(root.keys(0), 70);
    EXPECT_EQ(rightChild.keys(0), 80);
    EXPECT_EQ(rightChild.keys(1), 100);
}

TEST_F(BTreeTests, BorrowKeyRightTest)
{
    stu::btree tree;
    stu::btree::BNode root;
    stu::btree::BNode leftChild;
    stu::btree::BNode rightChild;

    leftChild.insertKey(50);

    rightChild.insertKey(80);
    rightChild.insertKey(90);
    rightChild.insertKey(100);

    root.insertKey(70);
    root.insertChild(&leftChild);
    root.insertChild(&rightChild);

    EXPECT_EQ(leftChild.size, 1);
    EXPECT_EQ(rightChild.size, 3);

    leftChild.borrowRight(&leftChild, &root, &rightChild);

    EXPECT_EQ(leftChild.size, 2);
    EXPECT_EQ(rightChild.size, 2);
    EXPECT_EQ(root.keys(0), 80);
    EXPECT_EQ(leftChild.keys(0), 50);
    EXPECT_EQ(leftChild.keys(1), 70);
}

/*TEST_F(BTreeTests, MergeTestLeaf)
{
    stu::btree tree;
    tree.insert(50);
    tree.insert(60);
    tree.insert(70);
    tree.insert(80);

    auto root = tree.getRoot();
    auto leftChild = root->children[0];
    auto rightChild = root->children[1];

    EXPECT_EQ(leftChild->size, 2);
    EXPECT_EQ(rightChild->size, 1);
    EXPECT_EQ(root->size, 1);

    tree.merge(root, 0);

    root = tree.getRoot();
    EXPECT_EQ(root->size, 4);
    EXPECT_EQ(root->childrenSize, 0);
    EXPECT_EQ(root->keys[0], 50);
    EXPECT_EQ(root->keys[1], 60);
    EXPECT_EQ(root->keys[2], 70);
    EXPECT_EQ(root->keys[3], 80);
}

TEST_F(BTreeTests, MergeTestInternal)
{
    stu::btree tree;
    tree.insert(50);
    tree.insert(60);
    tree.insert(70);
    tree.insert(80);
    tree.insert(90);
    tree.insert(100);
    tree.insert(110);
    tree.insert(120);
    tree.insert(130);
    tree.insert(140);
    tree.insert(150);
    tree.insert(160);
    tree.insert(170);

    auto root = const_cast<stu::btree::BNode*>(tree.getRoot());
    auto leftChild = root->children[0];
    auto rightChild = root->children[1];

    EXPECT_EQ(leftChild->size, 2);
    EXPECT_EQ(rightChild->size, 1);
    EXPECT_EQ(root->size, 1);

    tree.merge(root, 0);

    root = tree.getRoot();
    EXPECT_EQ(root->size, 4);
    EXPECT_EQ(root->childrenSize, 5);
    EXPECT_EQ(root->keys[0], 70);
    EXPECT_EQ(root->keys[1], 100);
    EXPECT_EQ(root->keys[2], 130);
    EXPECT_EQ(root->keys[3], 160);
}*/

TEST_F(BTreeTests, DeleteLeafSingle)
{
    stu::btree tree;
    tree.insert(100);

    EXPECT_EQ(tree.size(), 1);
    EXPECT_FALSE(tree.empty());

    tree.remove(100);

    EXPECT_EQ(tree.size(), 0);
    EXPECT_TRUE(tree.empty());
}

TEST_F(BTreeTests, DeleteLeafSufficient)
{
    stu::btree tree;
    tree.insert(100);
    tree.insert(50);
    tree.insert(150);
    tree.insert(200);

    EXPECT_EQ(tree.size(), 4);

    tree.remove(50);
    EXPECT_EQ(tree.size(), 3);
    EXPECT_FALSE(tree.search(50));

    tree.insert(50);
    tree.remove(100);

    EXPECT_FALSE(tree.search(100));

    EXPECT_TRUE(tree.search(50));
    EXPECT_TRUE(tree.search(150));
    EXPECT_TRUE(tree.search(200));
}

TEST_F(BTreeTests, DeleteLeafBorrow)
{
    stu::btree tree;
    tree.insert(100);
    tree.insert(50);
    tree.insert(150);
    tree.insert(200);

    EXPECT_EQ(tree.size(), 4);

    tree.remove(200);

    EXPECT_FALSE(tree.search(200));
    EXPECT_TRUE(tree.search(50));
    EXPECT_TRUE(tree.search(100));
    EXPECT_TRUE(tree.search(150));

    tree.insert(200);
    tree.remove(50);

    EXPECT_FALSE(tree.search(50));
    EXPECT_TRUE(tree.search(100));
    EXPECT_TRUE(tree.search(150));
    EXPECT_TRUE(tree.search(200));
}

stu::btree createTestTree()
{
    stu::btree tree;
    tree.insert(50);
    tree.insert(60);
    tree.insert(70);
    tree.insert(80);
    tree.insert(90);
    tree.insert(100);
    tree.insert(110);
    tree.insert(120);
    tree.insert(130);
    tree.insert(140);
    tree.insert(150);
    tree.insert(160);
    tree.insert(170);

    auto root = const_cast<stu::btree::BNode*>(tree.getRoot());
    auto leftChild = root->children(0);
    auto rightChild = root->children(1);

    // Ensure the initial state before removing
    EXPECT_EQ(leftChild->size, 2);
    EXPECT_EQ(rightChild->size, 1);
    EXPECT_EQ(root->size, 1);

    return tree;
}

/*TEST_F(BTreeTests, RemoveInternalTestBorrow)
{
    stu::btree tree = createTestTree();

    auto root = const_cast<stu::btree::BNode*>(tree.getRoot());
    auto leftChild = root->children[0];
    auto rightChild = root->children[1];

    // removeInternal, get predecessor
    tree.remove(160);
    tree.remove(130);

    // removeInternal, get successor
    tree.remove(50); // this removal is a leaf
    tree.remove(70);

    // Verify the state after removing
    EXPECT_EQ(rightChild->keys[0], 150);
    EXPECT_EQ(leftChild->keys[0], 80);
    EXPECT_EQ(root->keys[0], 120);
}*/

/*TEST_F(BTreeTests, RemoveInternalTestMerge)
{
    stu::btree tree = createTestTree();

    auto root = const_cast<stu::btree::BNode*>(tree.getRoot());
    auto leftChild = root->children[0];
    auto rightChild = root->children[1];

    // remove nodes to start merging
    tree.remove(160);
    tree.remove(130);
    tree.remove(50);

    // start
    tree.remove(70); // merge
    tree.remove(80);
    tree.remove(60);
    // tree.remove(100);   // merge
}*/

/*TEST_F(BTreeTests, RemoveInternalTestBorrow2)
{
    stu::btree tree = createTestTree();

    tree.insert(155);
    tree.insert(159);

    auto root = const_cast<stu::btree::BNode*>(tree.getRoot());
    auto leftChild = root->children[0];
    auto rightChild = root->children[1];

    // prepare for borrowing
    tree.remove(50);
    tree.remove(80);
    tree.remove(90);
    tree.remove(100);
    tree.remove(70);

    tree.remove(110);

    ASSERT_EQ(root->keys[0], 155);
    ASSERT_EQ(leftChild->keys[0], 130);
    ASSERT_EQ(rightChild->keys[0], 160);

    ASSERT_EQ(leftChild->children[0]->keys[0], 60);
    ASSERT_EQ(leftChild->children[0]->keys[1], 120);

    ASSERT_EQ(leftChild->children[1]->keys[0], 140);
    ASSERT_EQ(leftChild->children[1]->keys[1], 150);
}*/

/*TEST_F(BTreeTests, RemoveInternalTestMergeParent)
{
    stu::btree tree = createTestTree();

    // prepare for merging
    tree.remove(50);
    tree.remove(80);
    tree.remove(90);
    tree.remove(100);
    tree.remove(70);

    tree.remove(110);

    auto root = const_cast<stu::btree::BNode*>(tree.getRoot());
    auto firstChild = root->children[0];
    auto secondChild = root->children[1];
    auto thirdChild = root->children[2];
    auto fourthChild = root->children[3];

    ASSERT_EQ(root->keys[0], 110);
    ASSERT_EQ(root->keys[1], 130);
    ASSERT_EQ(root->keys[2], 160);

    ASSERT_EQ(firstChild->keys[0], 60);
    ASSERT_EQ(secondChild->keys[0], 120);
    ASSERT_EQ(thirdChild->keys[0], 140);
    ASSERT_EQ(fourthChild->keys[0], 170);
}*/

/* TEST_F(BTreeTests, RemoveInternalWithBorrowTest)
{
    stu::btree tree;
    tree.insert(50);5
    tree.insert(60);
    tree.insert(70);
    tree.insert(80);
    tree.insert(90);
    tree.insert(100);
    tree.insert(110);
    tree.insert(120);

    auto root = const_cast<stu::btree::BNode*>(tree.getRoot());
    auto leftChild = root->children[0];
    auto rightChild = root->children[1];

    // Ensure the initial state before removing
    EXPECT_EQ(leftChild->size, 2);
    EXPECT_EQ(rightChild->size, 2);
    EXPECT_EQ(root->size, 1);

    // Perform the removeInternal operation with borrowing
    tree.removeInternal(root, 70);

    // Verify the state after removing
    EXPECT_EQ(leftChild->size, 2);
    EXPECT_EQ(rightChild->size, 2);
    EXPECT_EQ(root->size, 0);
    EXPECT_EQ(leftChild->keys[0], 50);
    EXPECT_EQ(leftChild->keys[1], 60);
    EXPECT_EQ(rightChild->keys[0], 100);
    EXPECT_EQ(rightChild->keys[1], 110);
}

TEST_F(BTreeTests, RemoveInternalWithMergeTest)
{
    stu::btree tree;
    tree.insert(50);
    tree.insert(60);
    tree.insert(70);
    tree.insert(80);
    tree.insert(90);
    tree.insert(100);
    tree.insert(110);
    tree.insert(120);

    auto root = const_cast<stu::btree::BNode*>(tree.getRoot());
    auto leftChild = root->children[0];
    auto rightChild = root->children[1];

    // Ensure the initial state before removing
    EXPECT_EQ(leftChild->size, 2);
    EXPECT_EQ(rightChild->size, 2);
    EXPECT_EQ(root->size, 1);

    // Perform the removeInternal operation with merging
    tree.removeInternal(root, 60);

    // Verify the state after removing
    EXPECT_EQ(leftChild->size, 3);
    EXPECT_EQ(rightChild->size, 2);
    EXPECT_EQ(root->size, 0);
    EXPECT_EQ(leftChild->keys[0], 50);
    EXPECT_EQ(leftChild->keys[1], 70);
    EXPECT_EQ(leftChild->keys[2], 80);
    EXPECT_EQ(rightChild->keys[0], 100);
    EXPECT_EQ(rightChild->keys[1], 110);
}*/

void FileTest(std::string functionName, std::string_view fileName)
{
    std::cout << "\n" << functionName << "\n";

    std::ifstream file(fileName.data());
    ASSERT_TRUE(file.is_open());

    stu::btree tree;
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
        int value;
        int expected;

        iss >> command >> value;

        if (command == "INSERT")
        {
            auto start = std::chrono::high_resolution_clock::now();

            tree.insert(value);

            auto end = std::chrono::high_resolution_clock::now();
            insertTime += end - start;
        }
        else if (command == "DELETE")
        {
            auto start = std::chrono::high_resolution_clock::now();

            tree.remove(value);

            auto end = std::chrono::high_resolution_clock::now();
            removeTime += end - start;
        }
        else if (command == "SEARCH" && iss >> expected)
        {
            auto start = std::chrono::high_resolution_clock::now();

            bool contains = tree.search(value);

            auto end = std::chrono::high_resolution_clock::now();
            searchTime += end - start;

            ASSERT_EQ(contains, static_cast<bool>(expected));
        }
        else
        {
            break;
        }
    }

    ASSERT_TRUE(tree.isSorted());

    std::vector<int> expected = parseArray(line);
    std::vector<int> actual = tree.toVector();

    std::cout << "InsertTime: " << std::chrono::duration_cast<std::chrono::milliseconds>(insertTime).count() << "\n";
    std::cout << "SearchTime: " << std::chrono::duration_cast<std::chrono::milliseconds>(searchTime).count() << "\n";
    std::cout << "RemoveTime: " << std::chrono::duration_cast<std::chrono::milliseconds>(removeTime).count() << "\n";
    std::cout << "\n";

    EXPECT_EQ(expected, actual);
    // You can add assertions here to verify the state of the tree after insertion
}

// INSERT TESTS
TEST_F(BTreeTests, InsertTest)
{
    FileTest(__FUNCTION__, BTreeTests::insertFilePath);
}

//TEST_F(BTreeTests, InsertTestPerf)
//{
//    FileTest(__FUNCTION__, BTreeTests::perfInsertFilePath);
//}
//
//// SEARCH TESTS
//TEST_F(BTreeTests, SearchTest)
//{
//    FileTest(__FUNCTION__, BTreeTests::searchFilePath);
//}
//
//TEST_F(BTreeTests, SearchTestPerf)
//{
//    FileTest(__FUNCTION__, BTreeTests::perfSearchFilePath);
//}

//TEST_F(BTreeTests, RemoveTestFunctional100)
//{
//	FileTest(__FUNCTION__, BTreeTests::FunctionalRemove100FilePath);
//}
//
//TEST_F(BTreeTests, RemoveTestFunctional50)
//{
//	FileTest(__FUNCTION__, BTreeTests::FunctionalRemove50FilePath);
//}
//
//TEST_F(BTreeTests, RemoveTestNULL)
//{
//	FileTest(__FUNCTION__, BTreeTests::FunctionalRemoveNULLFilePath);
//}
//
//TEST_F(BTreeTests, RemoveFindPre)
//{
//	FileTest(__FUNCTION__, BTreeTests::funcRemoveFindPre);
//}

TEST_F(BTreeTests, RemoveTest)
{
	FileTest(__FUNCTION__, BTreeTests::removeFilePath);
}