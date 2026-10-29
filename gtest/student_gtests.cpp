#include <gtest/gtest.h>
#include <string.h>

#include <algorithm>

#include "string.hpp"
#include "alloc.hpp"

TEST(StringFunction, strlen) {
    EXPECT_EQ(String::strlen(""), 0);
    EXPECT_EQ(String::strlen("a"), 1);
    EXPECT_EQ(String::strlen("foo"), 3);
    EXPECT_EQ(String::strlen("hello world"), 11);   // space counts
    EXPECT_EQ(String::strlen("abc\0def"), 3);       // stops at first '\0'
}

TEST(StringFunction, strcpy) {
    char result[20];
    EXPECT_EQ(String::strcpy(result, "foo"), result);
    EXPECT_STREQ(result, "foo");

    // shorter copy over longer content: terminator must be written
    EXPECT_EQ(String::strcpy(result, "a"), result);
    EXPECT_STREQ(result, "a");

    EXPECT_EQ(String::strcpy(result, ""), result);
    EXPECT_STREQ(result, "");

    EXPECT_EQ(String::strcpy(result, "hello world"), result);
    EXPECT_STREQ(result, "hello world");
}

TEST(StringFunction, strdup) {
    const char *src = "hello";
    char *copy = String::strdup(src);
    EXPECT_STREQ(copy, "hello");
    EXPECT_NE(copy, src);              // new storage, not the same pointer
    copy[0] = 'J';                     // copy is independent of src
    EXPECT_STREQ(copy, "Jello");
    EXPECT_STREQ(src, "hello");
    delete[] copy;

    char *empty = String::strdup("");
    EXPECT_NE(empty, nullptr);
    EXPECT_STREQ(empty, "");
    delete[] empty;

    char *one = String::strdup("x");
    EXPECT_STREQ(one, "x");
    delete[] one;

    char *a = String::strdup("same");
    char *b = String::strdup("same");
    EXPECT_NE(a, b);                   // each call allocates its own array
    EXPECT_STREQ(a, b);
    delete[] a;
    delete[] b;
}

TEST(StringFunction, strncpy) {
    char result[20];

    // n smaller than src: copy n chars, then terminate
    EXPECT_EQ(String::strncpy(result, "hello", 3), result);
    EXPECT_STREQ(result, "hel");

    // n equal to src length
    EXPECT_EQ(String::strncpy(result, "hello", 5), result);
    EXPECT_STREQ(result, "hello");

    // n larger than src: stop at src's '\0'
    EXPECT_EQ(String::strncpy(result, "hi", 10), result);
    EXPECT_STREQ(result, "hi");

    // n == 0: empty result
    EXPECT_EQ(String::strncpy(result, "abc", 0), result);
    EXPECT_STREQ(result, "");

    // n == 1
    EXPECT_EQ(String::strncpy(result, "abc", 1), result);
    EXPECT_STREQ(result, "a");

    // empty src
    EXPECT_EQ(String::strncpy(result, "", 5), result);
    EXPECT_STREQ(result, "");

    // shorter copy over longer content: terminator must be written
    String::strcpy(result, "longer content");
    EXPECT_EQ(String::strncpy(result, "xyz", 2), result);
    EXPECT_STREQ(result, "xy");
}

TEST(StringFunction, strcat) {
    char result[30] = "foo";
    EXPECT_EQ(String::strcat(result, "bar"), result);
    EXPECT_STREQ(result, "foobar");

    // empty src leaves dest unchanged
    EXPECT_EQ(String::strcat(result, ""), result);
    EXPECT_STREQ(result, "foobar");

    // repeated appends go to the current end
    EXPECT_EQ(String::strcat(result, "!"), result);
    EXPECT_STREQ(result, "foobar!");

    // empty dest
    char empty[10] = "";
    EXPECT_EQ(String::strcat(empty, "abc"), empty);
    EXPECT_STREQ(empty, "abc");

    // both empty
    char none[5] = "";
    EXPECT_EQ(String::strcat(none, ""), none);
    EXPECT_STREQ(none, "");
}

TEST(StringFunction, strncat) {
    char result[30] = "foo";

    // n smaller than src: append n chars, then terminate
    EXPECT_EQ(String::strncat(result, "barbaz", 3), result);
    EXPECT_STREQ(result, "foobar");

    // n equal to src length
    EXPECT_EQ(String::strncat(result, "xy", 2), result);
    EXPECT_STREQ(result, "foobarxy");

    // n larger than src: stop at src's '\0'
    EXPECT_EQ(String::strncat(result, "!", 5), result);
    EXPECT_STREQ(result, "foobarxy!");

    // n == 0: nothing appended
    EXPECT_EQ(String::strncat(result, "zzz", 0), result);
    EXPECT_STREQ(result, "foobarxy!");

    // n == 1
    EXPECT_EQ(String::strncat(result, "qrs", 1), result);
    EXPECT_STREQ(result, "foobarxy!q");

    // empty dest
    char empty[10] = "";
    EXPECT_EQ(String::strncat(empty, "abcdef", 2), empty);
    EXPECT_STREQ(empty, "ab");

    // empty src
    char d[10] = "ab";
    EXPECT_EQ(String::strncat(d, "", 3), d);
    EXPECT_STREQ(d, "ab");
}

TEST(StringFunction, strcmp) {
    // equal
    EXPECT_EQ(String::strcmp("abc", "abc"), 0);
    EXPECT_EQ(String::strcmp("", ""), 0);
    EXPECT_EQ(String::strcmp("a", "a"), 0);

    // differ at first char, both signs
    EXPECT_LT(String::strcmp("abc", "bbc"), 0);
    EXPECT_GT(String::strcmp("bbc", "abc"), 0);

    // differ in the middle / at the end, both signs
    EXPECT_LT(String::strcmp("abc", "abd"), 0);
    EXPECT_GT(String::strcmp("abd", "abc"), 0);
    EXPECT_LT(String::strcmp("axc", "ayc"), 0);
    EXPECT_GT(String::strcmp("ayc", "axc"), 0);

    // prefix is smaller, both orders
    EXPECT_LT(String::strcmp("ab", "abc"), 0);
    EXPECT_GT(String::strcmp("abc", "ab"), 0);

    // empty vs non-empty, both orders
    EXPECT_LT(String::strcmp("", "a"), 0);
    EXPECT_GT(String::strcmp("a", ""), 0);

    // case matters: 'A' (65) < 'a' (97)
    EXPECT_LT(String::strcmp("Apple", "apple"), 0);
    EXPECT_GT(String::strcmp("apple", "Apple"), 0);
}

TEST(StringFunction, strncmp) {
    // equal within n even though they differ later
    EXPECT_EQ(String::strncmp("abcdef", "abcxyz", 3), 0);

    // first difference is exactly at position n-1
    EXPECT_LT(String::strncmp("abcdef", "abcxyz", 4), 0);
    EXPECT_GT(String::strncmp("abcxyz", "abcdef", 4), 0);

    // difference at first char, both signs
    EXPECT_LT(String::strncmp("abc", "xbc", 1), 0);
    EXPECT_GT(String::strncmp("xbc", "abc", 1), 0);

    // n == 0 always equal
    EXPECT_EQ(String::strncmp("abc", "xyz", 0), 0);
    EXPECT_EQ(String::strncmp("", "xyz", 0), 0);

    // n larger than both strings: stop at '\0'
    EXPECT_EQ(String::strncmp("ab", "ab", 10), 0);
    EXPECT_EQ(String::strncmp("", "", 5), 0);

    // prefix within n, both orders
    EXPECT_LT(String::strncmp("ab", "abc", 10), 0);
    EXPECT_GT(String::strncmp("abc", "ab", 10), 0);

    // prefix but n stops before the difference
    EXPECT_EQ(String::strncmp("ab", "abc", 2), 0);

    // empty vs non-empty with n > 0
    EXPECT_LT(String::strncmp("", "a", 3), 0);
    EXPECT_GT(String::strncmp("a", "", 3), 0);
}

TEST(StringFunction, reverse_cpy) {
    char result[20];

    // odd length
    String::reverse_cpy(result, "abc");
    EXPECT_STREQ(result, "cba");

    // even length
    String::reverse_cpy(result, "abcd");
    EXPECT_STREQ(result, "dcba");

    // single char
    String::reverse_cpy(result, "a");
    EXPECT_STREQ(result, "a");

    // empty
    String::reverse_cpy(result, "");
    EXPECT_STREQ(result, "");

    // terminator must be written over longer old content
    String::strcpy(result, "xxxxxxxxxx");
    String::reverse_cpy(result, "hello");
    EXPECT_STREQ(result, "olleh");

    // src is not modified
    const char *src = "stressed";
    String::reverse_cpy(result, src);
    EXPECT_STREQ(result, "desserts");
    EXPECT_STREQ(src, "stressed");
}

TEST(StringFunction, strchr) {
    const char *s = "hello";

    // match at the very start
    EXPECT_EQ(String::strchr(s, 'h'), s);

    // first of repeated chars
    EXPECT_EQ(String::strchr(s, 'l'), s + 2);

    // match at the last char
    EXPECT_EQ(String::strchr(s, 'o'), s + 4);

    // no match
    EXPECT_EQ(String::strchr(s, 'z'), nullptr);
    EXPECT_EQ(String::strchr(s, 'H'), nullptr);   // case matters

    // searching for '\0' finds the terminator
    EXPECT_EQ(String::strchr(s, '\0'), s + 5);

    // empty string
    const char *e = "";
    EXPECT_EQ(String::strchr(e, 'a'), nullptr);
    EXPECT_EQ(String::strchr(e, '\0'), e);
}

TEST(StringFunction, strstr) {
    const char *s = "hello world";

    // match at the very start
    EXPECT_EQ(String::strstr(s, "hello"), s);
    EXPECT_EQ(String::strstr(s, "h"), s);

    // match in the middle and at the end
    EXPECT_EQ(String::strstr(s, "o w"), s + 4);
    EXPECT_EQ(String::strstr(s, "world"), s + 6);
    EXPECT_EQ(String::strstr(s, "d"), s + 10);

    // first of several matches
    EXPECT_EQ(String::strstr(s, "o"), s + 4);

    // no match
    EXPECT_EQ(String::strstr(s, "xyz"), nullptr);
    EXPECT_EQ(String::strstr(s, "World"), nullptr);   // case matters

    // needle partly matches before it fully matches
    const char *t = "aaab";
    EXPECT_EQ(String::strstr(t, "aab"), t + 1);
    const char *u = "ababc";
    EXPECT_EQ(String::strstr(u, "abc"), u + 2);
    const char *v = "worwordword";
    EXPECT_EQ(String::strstr(v, "word"), v + 3);

    // partial match runs into the end of haystack
    EXPECT_EQ(String::strstr("abcab", "abd"), nullptr);
    EXPECT_EQ(String::strstr("hello", "lox"), nullptr);

    // needle longer than haystack
    EXPECT_EQ(String::strstr("ab", "abc"), nullptr);

    // needle equals haystack
    const char *w = "same";
    EXPECT_EQ(String::strstr(w, "same"), w);

    // empty needle matches at the start; empty haystack
    EXPECT_EQ(String::strstr(s, ""), s);
    const char *e = "";
    EXPECT_EQ(String::strstr(e, ""), e);
    EXPECT_EQ(String::strstr(e, "a"), nullptr);
}
