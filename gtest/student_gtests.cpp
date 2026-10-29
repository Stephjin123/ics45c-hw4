#include <gtest/gtest.h>
#include <string.h>

#include <algorithm>
#include <sstream>

#include "string.hpp"
#include "alloc.hpp"

// ===================== static helper functions =====================

TEST(StringFunction, strlen) {
    EXPECT_EQ(String::strlen(""), 0);
    EXPECT_EQ(String::strlen("foo"), 3);
    EXPECT_EQ(String::strlen("hello world"), 11);
}

TEST(StringFunction, strcpy) {
    char result[10];
    EXPECT_EQ(String::strcpy(result, "foo"), result);
    EXPECT_STREQ(result, "foo");

    EXPECT_EQ(String::strcpy(result, "a"), result);
    EXPECT_STREQ(result, "a");

    EXPECT_EQ(String::strcpy(result, ""), result);
    EXPECT_STREQ(result, "");
}

TEST(StringFunction, strdup) {
    const char *src = "hello";
    char *copy = String::strdup(src);
    EXPECT_STREQ(copy, "hello");
    EXPECT_NE(copy, src);           // must be new storage
    delete[] copy;

    char *empty = String::strdup("");
    EXPECT_STREQ(empty, "");
    delete[] empty;
}

TEST(StringFunction, strncpy) {
    char result[10];
    EXPECT_EQ(String::strncpy(result, "hello", 3), result);
    EXPECT_STREQ(result, "hel");     // homework version always terminates

    EXPECT_EQ(String::strncpy(result, "hi", 5), result);
    EXPECT_STREQ(result, "hi");      // stops early at src's '\0'

    EXPECT_EQ(String::strncpy(result, "abc", 0), result);
    EXPECT_STREQ(result, "");
}

TEST(StringFunction, strcat) {
    char result[20] = "foo";
    EXPECT_EQ(String::strcat(result, "bar"), result);
    EXPECT_STREQ(result, "foobar");

    EXPECT_EQ(String::strcat(result, ""), result);
    EXPECT_STREQ(result, "foobar");

    char empty[10] = "";
    EXPECT_EQ(String::strcat(empty, "abc"), empty);
    EXPECT_STREQ(empty, "abc");
}

TEST(StringFunction, strncat) {
    char result[20] = "foo";
    EXPECT_EQ(String::strncat(result, "barbaz", 3), result);
    EXPECT_STREQ(result, "foobar");

    EXPECT_EQ(String::strncat(result, "!", 5), result);
    EXPECT_STREQ(result, "foobar!");

    EXPECT_EQ(String::strncat(result, "xyz", 0), result);
    EXPECT_STREQ(result, "foobar!");
}

TEST(StringFunction, strcmp) {
    EXPECT_EQ(String::strcmp("abc", "abc"), 0);
    EXPECT_EQ(String::strcmp("", ""), 0);
    EXPECT_LT(String::strcmp("abc", "abd"), 0);
    EXPECT_GT(String::strcmp("abd", "abc"), 0);
    EXPECT_LT(String::strcmp("ab", "abc"), 0);   // prefix is smaller
    EXPECT_GT(String::strcmp("abc", "ab"), 0);
    EXPECT_LT(String::strcmp("", "a"), 0);
}

TEST(StringFunction, strncmp) {
    EXPECT_EQ(String::strncmp("abcdef", "abcxyz", 3), 0);
    EXPECT_LT(String::strncmp("abcdef", "abcxyz", 4), 0);
    EXPECT_GT(String::strncmp("abz", "aba", 3), 0);
    EXPECT_EQ(String::strncmp("abc", "xyz", 0), 0);
    EXPECT_EQ(String::strncmp("ab", "ab", 10), 0);   // stops at '\0'
    EXPECT_LT(String::strncmp("ab", "abc", 10), 0);
}

TEST(StringFunction, reverse_cpy) {
    char result[10];
    String::reverse_cpy(result, "abc");
    EXPECT_STREQ(result, "cba");

    String::reverse_cpy(result, "a");
    EXPECT_STREQ(result, "a");

    String::reverse_cpy(result, "");
    EXPECT_STREQ(result, "");
}

TEST(StringFunction, strchr) {
    const char *s = "hello";
    EXPECT_EQ(String::strchr(s, 'h'), s);
    EXPECT_EQ(String::strchr(s, 'l'), s + 2);    // first occurrence
    EXPECT_EQ(String::strchr(s, 'o'), s + 4);
    EXPECT_EQ(String::strchr(s, 'z'), nullptr);
    EXPECT_EQ(String::strchr(s, '\0'), s + 5);   // finds the terminator
    EXPECT_EQ(String::strchr("", 'a'), nullptr);
}

TEST(StringFunction, strstr) {
    const char *s = "hello world";
    EXPECT_EQ(String::strstr(s, "hello"), s);
    EXPECT_EQ(String::strstr(s, "world"), s + 6);
    EXPECT_EQ(String::strstr(s, "o"), s + 4);
    EXPECT_EQ(String::strstr(s, "xyz"), nullptr);
    EXPECT_EQ(String::strstr(s, ""), s);         // empty needle matches start
    EXPECT_EQ(String::strstr("ab", "abc"), nullptr);
    const char *t = "aaab";
    EXPECT_EQ(String::strstr(t, "aab"), t + 1);  // overlap / backtrack case
}

// ===================== String class (dynamic buf) =====================

TEST(StudentString, CopyIsIndependent) {
    String a("abc");
    String b(a);
    b[0] = 'x';
    EXPECT_EQ(a, String("abc"));     // changing the copy leaves a alone
    EXPECT_EQ(b, String("xbc"));
}

TEST(StudentString, AssignDifferentLengths) {
    String a("short");
    String b("a much longer string");
    a = b;
    EXPECT_EQ(a, b);
    EXPECT_EQ(a.size(), 20);

    b = String("x");
    EXPECT_EQ(b.size(), 1);
    EXPECT_EQ(a.size(), 20);         // a unaffected
}

TEST(StudentString, SelfAssignment) {
    String a("hello");
    a = a;
    EXPECT_EQ(a, String("hello"));
}

TEST(StudentString, PlusEqualsSelf) {
    String a("ab");
    a += a;
    EXPECT_EQ(a, String("abab"));
    EXPECT_EQ(a.size(), 4);
}

TEST(StudentString, PlusAndEmpty) {
    String a("foo");
    String empty;
    EXPECT_EQ(a + empty, String("foo"));
    EXPECT_EQ(empty + a, String("foo"));
    EXPECT_EQ(empty.size(), 0);
}

TEST(StudentString, Reverse) {
    String a("hello");
    EXPECT_EQ(a.reverse(), String("olleh"));
    EXPECT_EQ(a, String("hello"));   // reverse does not modify
    EXPECT_EQ(String("").reverse(), String(""));
}

TEST(StudentString, ReadWords) {
    std::istringstream in("first  second_longer_word x");
    String s;
    in >> s;
    EXPECT_EQ(s, String("first"));
    in >> s;
    EXPECT_EQ(s, String("second_longer_word"));
    in >> s;
    EXPECT_EQ(s, String("x"));
    EXPECT_EQ(s.size(), 1);
}

TEST(StudentString, PrintAndStream) {
    std::ostringstream out;
    String s("hi there");
    s.print(out);
    out << String("!");
    EXPECT_EQ(out.str(), "hi there!");
}
