#include <gtest/gtest.h>

#include <algorithm>
#include <sstream>

#include "string.hpp"

using namespace std;

TEST(StringClass, Constructors) {
    String s("hello");
    EXPECT_EQ(s.size(), 5);

    String t(s);
    EXPECT_EQ(t.size(), 5);
    EXPECT_EQ(s.size(), 5);
}

TEST(StringClass, Print) {
    String s("foo");
    stringstream out;
    s.print(out);
    EXPECT_STREQ(out.str().c_str(), "foo");

    stringstream out2;
    String().print(out2);
    EXPECT_STREQ(out2.str().c_str(), "");

    stringstream out3;
    out3 << s << s << String("bar");
    EXPECT_STREQ(out3.str().c_str(), "foofoobar");
}

TEST(StringClass, Comparisons) {
    EXPECT_LT(String("aaaa"), String("aaab"));
    EXPECT_LE(String("aaaa"), String("aaab"));
    EXPECT_EQ(String("foo"), String("foo"));
    EXPECT_NE(String("foo"), String("fooo"));
}

TEST(StringClass, Assignment) {
    String s("test"), t("foo");

    s = t;
    EXPECT_EQ(s.size(), t.size());
    EXPECT_EQ(s, t);

    s = s;
    EXPECT_EQ(s.size(), t.size());
    EXPECT_EQ(s, t);

    s = String();
    EXPECT_EQ(s, String());
    EXPECT_EQ(s.size(), 0);
}

// ===================== student tests: String member functions =====================
#include <sstream>

TEST(StudentString, CopyIsIndependent) {
    String a("abc");
    String b(a);
    b[0] = 'x';
    EXPECT_EQ(a, String("abc"));
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
    EXPECT_EQ(a.size(), 20);
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
    EXPECT_EQ(a, String("hello"));
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
