#include <fstream>
#include <string>
#include <string_view>
#include <chrono>

#include "ContainerTests.hpp"
#include "stu/bdd2.hpp"

class BDD2Tests : public ContainerTests
{
};

TEST_F(BDD2Tests, Default)
{
	std::cout << "\n\n\nWelcome to BDD tests\n\n\n";
}

TEST_F(BDD2Tests, CreateSingleVariableBDD)
{
	std::string formula = "x";
	std::string order = "x";

	stu::bdd2 bdd(formula, order);

	ASSERT_NE(bdd.getRoot(), nullptr);
	EXPECT_EQ(bdd.getRoot()->formula, "x");

	EXPECT_EQ(bdd.getRoot()->low->formula, "0");
	EXPECT_EQ(bdd.getRoot()->high->formula, "1");
}

TEST_F(BDD2Tests, CreateBasicPlusBDD)
{
	std::string formula = "x+y";
	std::string order = "xy";

	stu::bdd2 bdd(formula, order);
	EXPECT_EQ(bdd.getRoot()->formula, "x+y");

	EXPECT_EQ(bdd.getRoot()->low->formula, "y");
	EXPECT_EQ(bdd.getRoot()->high->formula, "1");

	ASSERT_NE(bdd.getRoot(), nullptr);
}

TEST_F(BDD2Tests, CreateBasicMultiplyBDD)
{
	std::string formula = "xy";
	std::string order = "xy";

	stu::bdd2 bdd(formula, order);

	ASSERT_NE(bdd.getRoot(), nullptr);
	EXPECT_EQ(bdd.getRoot()->formula, "xy");

	EXPECT_EQ(bdd.getRoot()->low->formula, "0");
	EXPECT_EQ(bdd.getRoot()->high->formula, "y");
}

TEST_F(BDD2Tests, CreateBddNegativeMultiply)
{
	std::string formula = "!xy";
	std::string order = "xy";

	stu::bdd2 bdd(formula, order);

	EXPECT_EQ(bdd.getRoot()->low->formula, "y");
	EXPECT_EQ(bdd.getRoot()->high->formula, "0");
}

TEST_F(BDD2Tests, CreateBddNegativePlus)
{
	std::string formula = "!x + y";
	std::string order = "xy";

	stu::bdd2 bdd(formula, order);

	EXPECT_EQ(bdd.getRoot()->low->formula, "1");
	EXPECT_EQ(bdd.getRoot()->high->formula, "y");
}

TEST_F(BDD2Tests, CreateBddNegativePlus2)
{
	std::string formula = "x + !y";
	std::string order = "xy";

	stu::bdd2 bdd(formula, order);

	EXPECT_EQ(bdd.getRoot()->low->formula, "!y");
	EXPECT_EQ(bdd.getRoot()->low->low->formula, "1");
	EXPECT_EQ(bdd.getRoot()->low->high->formula, "0");
	EXPECT_EQ(bdd.getRoot()->high->formula, "1");
}

TEST_F(BDD2Tests, UseSingleVariableBDD)
{
	std::string formula = "x";
	std::string order = "x";

	stu::bdd2 bdd(formula, order);

	EXPECT_TRUE(bdd.use("1"));
	EXPECT_FALSE(bdd.use("0"));
}

TEST_F(BDD2Tests, UseTwoVariableBDD)
{
	std::string formula = "x+y";
	std::string order = "xy";

	stu::bdd2 bdd(formula, order);

	EXPECT_TRUE(bdd.use("10"));
	EXPECT_TRUE(bdd.use("01"));
	EXPECT_TRUE(bdd.use("11"));
	EXPECT_FALSE(bdd.use("00"));
}

TEST_F(BDD2Tests, UseNegationBDD)
{
	std::string formula = "!x+y";
	std::string order = "xy";

	stu::bdd2 bdd(formula, order);

	EXPECT_TRUE(bdd.use("01"));
	EXPECT_TRUE(bdd.use("00"));
	EXPECT_FALSE(bdd.use("10"));
	EXPECT_TRUE(bdd.use("11"));
}

// Test: Evaluate a constant BDD
TEST_F(BDD2Tests, UseConstantBDD)
{
	std::string formula = "1";
	std::string order = "";

	stu::bdd2 bdd(formula, order);

	EXPECT_TRUE(bdd.use(""));
}

// Test: Invalid input length
TEST_F(BDD2Tests, UseInvalidInputLength)
{
	std::string formula = "x+y";
	std::string order = "xy";

	stu::bdd2 bdd(formula, order);

	EXPECT_FALSE(bdd.use("1"));
}

void testFromFileBdd2(std::string functionName, std::string_view fileName)
{
	using clock = std::chrono::high_resolution_clock;
	using milliseconds = std::chrono::milliseconds;
	using nanoseconds = std::chrono::nanoseconds;

	std::cout << "\n" << functionName << "\n";
	std::ifstream file(fileName.data());
	if (!file.is_open())
	{
		std::cout << "\n\n\nERROR : File was not opened:" << fileName << "\n\n\n";
	}
	ASSERT_TRUE(file.is_open());

	// Time Measurements
	nanoseconds useTime{};
	nanoseconds createTime{};

	size_t useCount{};
	size_t createCount = 2000;


	// Initialize BDD
	std::string formula;
	std::string order;

	std::getline(file, formula);
	std::getline(file, order);

	stu::bdd2 bdd(formula, order);
	double beforeReduction = bdd.size();

	auto start = clock::now();
	double bestReduction = bdd.createBestOrder(formula, order, createCount);
	auto end = clock::now();

	std::cout << "Before reduction: " << beforeReduction << "\n";
	std::cout << "Best reduction: " << bestReduction << "\n";
	std::cout << "Reduction rate: " <<  beforeReduction / bestReduction << "\n\n";

	createTime += end - start;

	std::string line;

	while (std::getline(file, line))
	{
		std::istringstream iss(line);

		std::string input;
		bool value;
		char delim;

		iss >> input >> delim >> value;
		ASSERT_EQ(delim, '-');


		auto start = clock::now();
		bool result = bdd.use(input);
		auto end = clock::now();

		useTime += end - start;
		useCount++;

		if (value != result)
		{
			std::cout << "ERROR" << "\n\n\n\n\n\n\n\n";
			bool result = bdd.use(input);
		}

		ASSERT_EQ(value, result);
	}

	// Measurments logging
	createCount = std::min(createCount, stu::fac(bdd.getOrder().size()));

	milliseconds createTimeMillis = std::chrono::duration_cast<milliseconds>(createTime);
	milliseconds useTimeMillis = std::chrono::duration_cast<milliseconds>(useTime);

	std::cout << "For BDD with max depth of " << bdd.getOrder().size() << ":\n";
	std::cout << "create() was invoked " << createCount << " times. This took " << createTimeMillis << " milliseconds.\n";
	std::cout << "use() was invoked " << useCount << " times. This took " << useTimeMillis << " milliseconds.\n\n";
}


//// Functional tests for debugging
//TEST_F(BDD2Tests, FileTestFunc1)
//{
//	testFromFileBdd2(__FUNCTION__, "resources/bdd_test_func1.txt");
//}
//
//TEST_F(BDD2Tests, FileTestFunc2)
//{
//	testFromFileBdd2(__FUNCTION__, "resources/bdd_test_func2.txt");
//}
//
//TEST_F(BDD2Tests, FileTestFunc3)
//{
//	testFromFileBdd2(__FUNCTION__, "resources/bdd_test_func3.txt");
//}
//
TEST_F(BDD2Tests, FileTestFunc4)
{
	testFromFileBdd2(__FUNCTION__, "resources/bdd_test_func4.txt");
}
//
//TEST_F(BDD2Tests, FileTest)
//{
//	testFromFileBdd2(__FUNCTION__, "resources/bdd_test_output.txt");
//}

TEST_F(BDD2Tests, FileTestFunc10_20)
{
	testFromFileBdd2(__FUNCTION__, "resources/bdd_test_10_20.txt");
}

TEST_F(BDD2Tests, FileTestFunc15_30)
{
	testFromFileBdd2(__FUNCTION__, "resources/bdd_test_15_30.txt");
}

TEST_F(BDD2Tests, FileTestFunc20_40)
{
	testFromFileBdd2(__FUNCTION__, "resources/bdd_test_20_40.txt");
}

TEST_F(BDD2Tests, FileTestFunc22_40)
{
	testFromFileBdd2(__FUNCTION__, "resources/bdd_test_22_44.txt");
}

TEST_F(BDD2Tests, FileTestFunc24_48)
{
	testFromFileBdd2(__FUNCTION__, "resources/bdd_test_24_48.txt");
}
//
//TEST_F(BDD2Tests, FileTestFunc10_50)
//{
//	testFromFileBdd2(__FUNCTION__, "resources/bdd_test_10_50.txt");
//}
//
//TEST_F(BDD2Tests, FileTestFunc15_75)
//{
//	testFromFileBdd2(__FUNCTION__, "resources/bdd_test_15_75.txt");
//}
//
//TEST_F(BDD2Tests, FileTestFunc20_100)
//{
//	testFromFileBdd2(__FUNCTION__, "resources/bdd_test_20_100.txt");
//}
//
//TEST_F(BDD2Tests, FileTestFunc22_110)
//{
//	testFromFileBdd2(__FUNCTION__, "resources/bdd_test_22_110.txt");
//}
//
//TEST_F(BDD2Tests, FileTestFunc24_120)
//{
//	testFromFileBdd2(__FUNCTION__, "resources/bdd_test_24_120.txt");
//}