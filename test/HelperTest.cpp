#include "Helper.h"
#include <gtest/gtest.h>
#include <vector>
#include <string>

TEST(HelperTest, Split_Basic) {
    const std::vector<std::string> expected{"api", "user"};
    EXPECT_EQ(Helper::split("/api/user", '/'), expected)
        << "Leading empty segment from the leading '/' is dropped";
}

TEST(HelperTest, Split_Skips_Empty_Segments) {
    const std::vector<std::string> expected{"a", "b"};
    EXPECT_EQ(Helper::split("a//b", '/'), expected) << "Consecutive delimiters produce no empty entries";
    EXPECT_EQ(Helper::split("///", '/'), std::vector<std::string>{}) << "Only delimiters yields nothing";
    EXPECT_EQ(Helper::split("", '/'), std::vector<std::string>{}) << "Empty input yields nothing";
}

TEST(HelperTest, Trim_Removes_Surrounding_Whitespace) {
    EXPECT_EQ(Helper::trim("  hello  "), "hello");
    EXPECT_EQ(Helper::trim("\t\n hello \r\n"), "hello") << "Tabs and newlines count as whitespace";
    EXPECT_EQ(Helper::trim("hello"), "hello") << "No change when nothing to trim";
    EXPECT_EQ(Helper::trim("   "), "") << "All-whitespace becomes empty";
    EXPECT_EQ(Helper::trim("  a b  "), "a b") << "Internal whitespace is preserved";
}

TEST(HelperTest, ToLower_Lowercases_Ascii) {
    EXPECT_EQ(Helper::toLower("HELLO"), "hello");
    EXPECT_EQ(Helper::toLower("HeLLo123"), "hello123") << "Digits are left untouched";
    EXPECT_EQ(Helper::toLower("already"), "already");
}
