#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <iostream>

#include "SimpleCalculator.h"
#include "InMemoryHistory.h"
#include "IHistory.h"
#include <limits>

class CalcTest : public ::testing::Test
{
public:
    static void setupTestSuite()
    {
    }

    void SetUp() override
    {
        hist = std::make_unique<calc::InMemoryHistory>();
        calc = std::make_unique<calc::SimpleCalculator>(*hist);
    }

    void TearDown() override
    {
        calc.reset();
        hist.reset();
    }

    static void TearDownTestSuite()
    {
    }

    std::unique_ptr<calc::InMemoryHistory> hist;
    std::unique_ptr<calc::SimpleCalculator> calc;
    const int MAX_INT = std::numeric_limits<int>::max();
    const int MIN_INT = std::numeric_limits<int>::min();
};

TEST_F(CalcTest, BasicTest)
{
    EXPECT_EQ(1, 1);
    EXPECT_EQ(true, true);
    EXPECT_EQ(5 + 8, 13);
    EXPECT_NE(5 + 7, 13);
    EXPECT_EQ(5 * 8, 40);
    EXPECT_NE(5 * 7, 40);
    EXPECT_STREQ("40", "40");
    EXPECT_STRNE("5*8", "40");
}

TEST_F(CalcTest, BasicTestCalc)
{
    EXPECT_EQ(calc->Add(5, 8), 13);
    EXPECT_EQ(calc->Add(5, 7), 12);
    EXPECT_EQ(calc->Subtract(5, 7), -2);
    EXPECT_EQ(calc->Multiply(5, 8), 40);
    EXPECT_EQ(calc->Divide(5, 8), 0);
    EXPECT_EQ(calc->Divide(100, 8), 12);
    EXPECT_EQ(calc->Divide(100, 1), 100);
}

TEST_F(CalcTest, TestAdd)
{
    EXPECT_EQ(calc->Add(455, 565), 1020);
    EXPECT_EQ(calc->Add(999, 0), 999);
    EXPECT_EQ(calc->Add(0, 0), 0);
    EXPECT_EQ(calc->Add(MAX_INT, 1), MIN_INT); // Overflow
}

TEST_F(CalcTest, TestSub)
{
    EXPECT_EQ(calc->Subtract(455, 565), -110);
    EXPECT_EQ(calc->Subtract(999, 0), 999);
    EXPECT_EQ(calc->Subtract(0, 999), -999);
    EXPECT_EQ(calc->Subtract(0, 0), 0);
}

TEST_F(CalcTest, TestMul)
{
    EXPECT_EQ(calc->Multiply(455, 565), 257075);
    EXPECT_EQ(calc->Multiply(999, 0), 0);
    EXPECT_EQ(calc->Multiply(0, 999), 0);
    EXPECT_EQ(calc->Multiply(0, 0), 0);
}

TEST_F(CalcTest, TestDivide)
{
    EXPECT_EQ(calc->Divide(455, 565), 0);
    EXPECT_EQ(calc->Divide(999, 1), 999);
    EXPECT_EQ(calc->Divide(52, -6), -8);
    EXPECT_EQ(calc->Divide(-52, 6), -8);
    EXPECT_EQ(calc->Divide(-52, -6), 8);
    EXPECT_EQ(calc->Divide(0, 999), 0);
}

TEST_F(CalcTest, TestDivideZero)
{
    EXPECT_THROW(calc->Divide(100, 0), std::runtime_error);
    EXPECT_THROW(calc->Divide(0, 0), std::runtime_error);
}

TEST_F(CalcTest, TestSubOverflow)
{
    EXPECT_THROW(calc->Subtract(MIN_INT, 1), std::overflow_error) << "No checking for overflow";
}

TEST_F(CalcTest, TestAddOverflow)
{
    EXPECT_THROW(calc->Add(MAX_INT, 1), std::overflow_error) << "No checking for overflow";
}

TEST_F(CalcTest, TestMulOverflow)
{
    EXPECT_THROW(calc->Multiply(MAX_INT, 2), std::overflow_error) << "No checking for overflow";
}

TEST_F(CalcTest, TestHistory)
{
    calc->Add(999, 999);
    calc->Multiply(999, 999);
    calc->Divide(999, 999);
    calc->Subtract(999, 998);
    std::vector<std::string> operations = hist->GetLastOperations(4);

    ASSERT_EQ(operations.size(), 4);

    EXPECT_STRCASEEQ(operations[0].c_str(), "999 + 999 = 1998");
    EXPECT_STRCASEEQ(operations[1].c_str(), "999 * 999 = 998001");
    EXPECT_STRCASEEQ(operations[2].c_str(), "999 / 999 = 1");
    EXPECT_STRCASEEQ(operations[3].c_str(), "999 - 998 = 1");
}

class HistoryMock : public calc::IHistory
{
public:
    MOCK_METHOD(void, AddEntry, (const std::string &), (override));
    MOCK_METHOD(std::vector<std::string>, GetLastOperations, (size_t), (const, override));
};

class CalcHistoryMockTest : public ::testing::Test
{
public:
    static void setupTestSuite()
    {
    }

    void SetUp() override
    {
        histMock = std::make_unique<HistoryMock>();
        calc = std::make_unique<calc::SimpleCalculator>(*histMock);
    }

    void TearDown() override
    {
        calc.reset();
        histMock.reset();
    }

    static void TearDownTestSuite()
    {
    }

    std::unique_ptr<HistoryMock> histMock;
    std::unique_ptr<calc::SimpleCalculator> calc;
    const int MAX_INT = std::numeric_limits<int>::max();
    const int MIN_INT = std::numeric_limits<int>::min();
};

TEST_F(CalcHistoryMockTest, TestAddEntryCall)
{
    EXPECT_CALL(*histMock, AddEntry(testing::HasSubstr("*"))).Times(1);
    calc->Multiply(6, 7);
    EXPECT_CALL(*histMock, AddEntry(testing::HasSubstr("+"))).Times(1);
    calc->Add(6, 7);
    EXPECT_CALL(*histMock, AddEntry(testing::HasSubstr("-"))).Times(1);
    calc->Subtract(6, 7);
    EXPECT_CALL(*histMock, AddEntry(testing::HasSubstr("/"))).Times(1);
    calc->Divide(6, 7);
}