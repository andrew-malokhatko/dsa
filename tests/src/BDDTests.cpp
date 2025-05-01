//
//#include <fstream>
//
//#include "ContainerTests.hpp"
//#include "stu/bdd.hpp"
//
//class BDDTests : public ContainerTests
//{
//
//};
//
//TEST_F(BDDTests, Default)
//{
//	std::cout << "\n\n\nWelcome to BDD tests\n\n\n";
//}
//
//TEST_F(BDDTests, BddIsNegative)
//{
//	stu::bdd bdd("", "");
//
//	EXPECT_TRUE(bdd.isNegative("!x", 'x'));
//	EXPECT_TRUE(bdd.isNegative("y+!x", 'x'));
//	EXPECT_TRUE(bdd.isNegative("!x+z", 'x'));
//
//	EXPECT_FALSE(bdd.isNegative("x", 'x'));
//	EXPECT_FALSE(bdd.isNegative("y+x", 'x'));
//	EXPECT_FALSE(bdd.isNegative("x+z", 'x'));
//
//	EXPECT_FALSE(bdd.isNegative("y+z", 'x'));
//	EXPECT_FALSE(bdd.isNegative("!y+z", 'x'));
//
//	EXPECT_FALSE(bdd.isNegative("", 'x'));   // Empty formula
//	EXPECT_FALSE(bdd.isNegative("!", 'x'));  // Only negation symbol
//	EXPECT_FALSE(bdd.isNegative("!y", 'x')); // Negation of a different variable
//}
//
//TEST_F(BDDTests, CreateSingleVariableBDD)
//{
//	std::string formula = "x";
//	std::string order = "x";
//
//	stu::bdd bdd(formula, order);
//
//	ASSERT_NE(bdd.getRoot(), nullptr);
//	EXPECT_EQ(bdd.getRoot()->formula, "x");
//
//	EXPECT_EQ(bdd.getRoot()->low->formula, "0");
//	EXPECT_EQ(bdd.getRoot()->high->formula, "1");
//}
//
//TEST_F(BDDTests, CreateBasicPlusBDD)
//{
//	std::string formula = "x+y";
//	std::string order = "xy";
//
//	stu::bdd bdd(formula, order);
//
//	ASSERT_NE(bdd.getRoot(), nullptr);
//	EXPECT_EQ(bdd.getRoot()->formula, "x+y");
//
//	EXPECT_EQ(bdd.getRoot()->low->formula, "y");
//	EXPECT_EQ(bdd.getRoot()->high->formula, "1");
//}
//
//TEST_F(BDDTests, CreateBasicMultiplyBDD)
//{
//	std::string formula = "xy";
//	std::string order = "xy";
//
//	stu::bdd bdd(formula, order);
//
//	ASSERT_NE(bdd.getRoot(), nullptr);
//	EXPECT_EQ(bdd.getRoot()->formula, "xy");
//
//	EXPECT_EQ(bdd.getRoot()->low->formula, "0");
//	EXPECT_EQ(bdd.getRoot()->high->formula, "y");
//}
//
//TEST_F(BDDTests, CreateBddNegativeMultiply)
//{
//	std::string formula = "!xy";
//	std::string order = "xy";
//
//	stu::bdd bdd(formula, order);
//
//	EXPECT_EQ(bdd.getRoot()->low->formula, "y");
//	EXPECT_EQ(bdd.getRoot()->high->formula, "0");
//}
//
//TEST_F(BDDTests, CreateBddNegativePlus)
//{
//	std::string formula = "!x + y";
//	std::string order = "xy";
//
//	stu::bdd bdd(formula, order);
//
//	EXPECT_EQ(bdd.getRoot()->low->formula, "1");
//	EXPECT_EQ(bdd.getRoot()->high->formula, "y");
//}
//
//TEST_F(BDDTests, CreateBddNegativePlus2)
//{
//	std::string formula = "x + !y";
//	std::string order = "xy";
//
//	stu::bdd bdd(formula, order);
//
//	EXPECT_EQ(bdd.getRoot()->low->formula, "!y");
//	EXPECT_EQ(bdd.getRoot()->low->low->formula, "1");
//	EXPECT_EQ(bdd.getRoot()->low->high->formula, "0");
//	EXPECT_EQ(bdd.getRoot()->high->formula, "1");
//}
//
//TEST_F(BDDTests, ReductionTestTypeSPlusInRoot)
//{
//	std::string formula = "x + !x";
//	std::string order = "x";
//
//	stu::bdd bdd(formula, order);
//
//	EXPECT_EQ(bdd.getRoot()->formula, "1");
//}
//
//TEST_F(BDDTests, ReductionTestTypeSPlus)
//{
//	std::string formula = "xy + !x + !y";
//	std::string order = "xy";
//
//	stu::bdd bdd(formula, order);
//
//	EXPECT_EQ(bdd.getRoot()->formula, "1");
//}
//
//
//TEST_F(BDDTests, UseSingleVariableBDD)
//{
//	std::string formula = "x";
//	std::string order = "x";
//
//	stu::bdd bdd(formula, order);
//
//	EXPECT_TRUE(bdd.use("1"));
//	EXPECT_FALSE(bdd.use("0"));
//}
//
//TEST_F(BDDTests, UseTwoVariableBDD)
//{
//	std::string formula = "x+y";
//	std::string order = "xy";
//
//	stu::bdd bdd(formula, order);
//
//	EXPECT_TRUE(bdd.use("10"));
//	EXPECT_TRUE(bdd.use("01"));
//	EXPECT_TRUE(bdd.use("11"));
//	EXPECT_FALSE(bdd.use("00"));
//}
//
//TEST_F(BDDTests, UseNegationBDD)
//{
//	std::string formula = "!x+y";
//	std::string order = "xy";
//
//	stu::bdd bdd(formula, order);
//
//	EXPECT_TRUE(bdd.use("01"));
//	EXPECT_TRUE(bdd.use("00"));
//	EXPECT_FALSE(bdd.use("10"));
//	EXPECT_TRUE(bdd.use("11"));
//}
//
//// Test: Evaluate a constant BDD
//TEST_F(BDDTests, UseConstantBDD)
//{
//	std::string formula = "1";
//	std::string order = "";
//
//	stu::bdd bdd(formula, order);
//
//	EXPECT_TRUE(bdd.use(""));
//}
//
//// Test: Invalid input length
//TEST_F(BDDTests, UseInvalidInputLength)
//{
//	std::string formula = "x+y";
//	std::string order = "xy";
//
//	stu::bdd bdd(formula, order);
//
//	EXPECT_FALSE(bdd.use("1"));
//}
//
//void testFromFile(std::string functionName, std::string_view fileName)
//{
//	std::cout << "\n" << functionName << "\n";
//
//	std::ifstream file(fileName.data());
//	if (!file.is_open())
//	{
//		std::cout << "\n\n\nERROR : File was not opened\n\n\n";
//	}
//
//	ASSERT_TRUE(file.is_open());
//
//
//	// Initialize BDD
//	std::string formula;
//	std::string order;
//
//	std::getline(file, formula);
//	std::getline(file, order);
//
//	stu::bdd bdd(formula, order);
//
//	// check if it's correct
//	std::string line;
//
//	while (std::getline(file, line))
//	{
//		std::istringstream iss(line);
//
//		std::string input;
//		bool value;
//		char delim;
//
//		iss >> input >> delim >> value;
//		ASSERT_EQ(delim, '-');
//
//		bool result = bdd.use(input);
//
//		if (value != result)
//		{
//			std::cout << "ERROR" << "\n\n\n\n\n\n\n\n";
//			bool result = bdd.use(input);
//		}
//
//		ASSERT_EQ(value, result);
//	}
//}
//
//
//// functional test
//TEST_F(BDDTests, FileTestFunc1)
//{
//	testFromFile(__FUNCTION__, "resources/bdd_test_func1.txt");
//}
//
//// functional test 2
//TEST_F(BDDTests, FileTestFunc2)
//{
//	testFromFile(__FUNCTION__, "resources/bdd_test_func2.txt");
//}
//
//TEST_F(BDDTests, FileTestFunc3)
//{
//	testFromFile(__FUNCTION__, "resources/bdd_test_func3.txt");
//} 
//
//TEST_F(BDDTests, FileTestFunc4)
//{
//	testFromFile(__FUNCTION__, "resources/bdd_test_func4.txt");
//} 
//
////TEST_F(BDDTests, FileTest)
////{
////	testFromFile(__FUNCTION__, "resources/bdd_test_output.txt");
////}