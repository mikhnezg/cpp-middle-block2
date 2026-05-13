#include <gtest/gtest.h>
#include <print>

#include "scan.hpp"

TEST(ScanTest, SimpleTest) {
    auto result = stdx::scan<std::string>("number", "{}");
    ASSERT_TRUE(result);
    auto [str] = result->values();
    ASSERT_EQ(str, "number");
}

TEST(ScanTest, IntTest) {
    auto result = stdx::scan<int32_t>("42", "{d}");
    ASSERT_TRUE(result);
    auto [val] = result->values();
    ASSERT_EQ(val, 42);
}

TEST(ScanTest, FloatTest) {
    auto result = stdx::scan<float>("3.14", "{f}");
    ASSERT_TRUE(result);
    auto [val] = result->values();
    ASSERT_FLOAT_EQ(val, 3.14f);
}

TEST(ScanTest, MultipleTest) {
    auto result =
        stdx::scan<int, float, std::string>("I want to sum 42 and 3.14 numbers.", "I want to sum {d} and {f} {}");
    ASSERT_TRUE(result);
    auto [i, f, s] = result->values();
    ASSERT_EQ(i, 42);
    ASSERT_FLOAT_EQ(f, 3.14f);
    ASSERT_EQ(s, "numbers.");
}

TEST(ScanTest, ErrorTest) {
    auto result = stdx::scan<int>("not a number", "{d}");
    ASSERT_FALSE(result);
}

TEST(ScanTest, Int8Test) {
    auto result = stdx::scan<int8_t>("-10", "{d}");
    ASSERT_TRUE(result);
    auto [val] = result->values();
    ASSERT_EQ(val, -10);
}

TEST(ScanTest, Uint64Test) {
    auto result = stdx::scan<uint64_t>("18446744073709551615", "{u}");
    ASSERT_TRUE(result);
    auto [val] = result->values();
    ASSERT_EQ(val, 18446744073709551615ULL);
}

TEST(ScanTest, DoubleTest) {
    auto result = stdx::scan<double>("2.71828", "{f}");
    ASSERT_TRUE(result);
    auto [val] = result->values();
    ASSERT_DOUBLE_EQ(val, 2.71828);
}

TEST(ScanTest, StringViewTest) {
    auto result = stdx::scan<std::string_view>("hello", "{s}");
    ASSERT_TRUE(result);
    auto [str] = result->values();
    ASSERT_EQ(str, "hello");
}

TEST(ScanTest, MultipleIntsTest) {
    auto result = stdx::scan<int16_t, int64_t>("Values: 100 200", "Values: {d} {d}");
    ASSERT_TRUE(result);
    auto [a, b] = result->values();
    ASSERT_EQ(a, 100);
    ASSERT_EQ(b, 200);
}

TEST(ScanTest, UnsignedTest) {
    auto result = stdx::scan<uint32_t>("4294967295", "{u}");
    ASSERT_TRUE(result);
    auto [val] = result->values();
    ASSERT_EQ(val, 4294967295U);
}

TEST(ScanTest, ErrorMismatchType) {
    auto result = stdx::scan<int>("3.14", "{d}");  // float in int
    ASSERT_FALSE(result);
}

TEST(ScanTest, ErrorWrongSpecifier) {
    auto result = stdx::scan<int>("42", "{x}");  // unknown specifier
    ASSERT_FALSE(result);
}

TEST(ScanTest, ErrorTooManyPlaceholders) {
    auto result = stdx::scan<int>("42", "{} {}");  // more placeholders than types
    ASSERT_FALSE(result);
}

TEST(ScanTest, ErrorTooFewPlaceholders) {
    auto result = stdx::scan<int, int>("42", "{}");  // fewer placeholders
    ASSERT_FALSE(result);
}