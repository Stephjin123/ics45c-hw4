#include <iostream>
#include <iomanip>
#include <utility>
#include "string.hpp"
using namespace std;

// ===================== static helper functions =====================
// All loops through C strings live here; the interface methods below
// only call these helpers.

int String::strlen(const char *s) {
    int len = 0;
    while (s[len] != '\0')
        ++len;
    return len;
}

char *String::strcpy(char *dest, const char *src) {
    int i = 0;
    for (; src[i] != '\0'; ++i)
        dest[i] = src[i];
    dest[i] = '\0';
    return dest;
}

// allocates exactly enough room for src plus its terminator
char *String::strdup(const char *src) {
    return strcpy(new char[strlen(src) + 1], src);
}

// Homework version (NOT the C standard one): copies at most n chars,
// then ALWAYS adds a terminator, so dest needs room for n + 1 chars.
char *String::strncpy(char *dest, const char *src, int n) {
    int i = 0;
    for (; i < n && src[i] != '\0'; ++i)
        dest[i] = src[i];
    dest[i] = '\0';
    return dest;
}

char *String::strcat(char *dest, const char *src) {
    strcpy(dest + strlen(dest), src);
    return dest;
}

// Appends at most n chars of src, then always adds a terminator.
char *String::strncat(char *dest, const char *src, int n) {
    strncpy(dest + strlen(dest), src, n);
    return dest;
}

// < 0 if left < right, 0 if equal, > 0 if left > right
int String::strcmp(const char *left, const char *right) {
    int i = 0;
    while (left[i] != '\0' && left[i] == right[i])
        ++i;
    return (unsigned char)left[i] - (unsigned char)right[i];
}

int String::strncmp(const char *left, const char *right, int n) {
    for (int i = 0; i < n; ++i) {
        if (left[i] != right[i] || left[i] == '\0')
            return (unsigned char)left[i] - (unsigned char)right[i];
    }
    return 0;
}

void String::reverse_cpy(char *dest, const char *src) {
    int len = strlen(src);
    for (int i = 0; i < len; ++i)
        dest[i] = src[len - 1 - i];
    dest[len] = '\0';
}

// Searching for '\0' returns a pointer to the terminator.
const char *String::strchr(const char *str, char c) {
    for (int i = 0; ; ++i) {
        if (str[i] == c)
            return str + i;
        if (str[i] == '\0')
            return nullptr;
    }
}

// An empty needle matches at the start of haystack.
const char *String::strstr(const char *haystack, const char *needle) {
    if (needle[0] == '\0')
        return haystack;
    for (int start = 0; haystack[start] != '\0'; ++start) {
        int i = 0;
        // reaching haystack's '\0' is always a mismatch,
        // so we never read past the end of haystack
        while (needle[i] != '\0' && haystack[start + i] == needle[i])
            ++i;
        if (needle[i] == '\0')
            return haystack + start;
    }
    return nullptr;
}

// ===================== constructors / destructor =====================

// the default argument "" appears only in the declaration
String::String(const char *s)
    : buf(strdup(s)) {}

String::String(const String &s)
    : buf(strdup(s.buf)) {}

// private: room for length chars plus '\0'; contents not initialized,
// the caller must fill them in and terminate
String::String(int length)
    : buf(new char[length + 1]) {}

String::~String() {
    delete[] buf;
}

// ===================== swap / assignment =====================

void String::swap(String &s) {
    std::swap(buf, s.buf);
}

// s is already a copy; take its buf, and s's destructor frees our old one
String &String::operator=(String s) {
    swap(s);
    return *this;
}

// ===================== indexing / queries =====================

char &String::operator[](int index) {
    if (!in_bounds(index)) {
        cout << "ERROR: Index Out Of Bounds" << endl;
        return buf[0];
    }
    return buf[index];
}

const char &String::operator[](int index) const {
    if (!in_bounds(index)) {
        cout << "ERROR: Index Out Of Bounds" << endl;
        return buf[0];
    }
    return buf[index];
}

int String::size() const {
    return strlen(buf);
}

String String::reverse() const {
    String result(size());
    reverse_cpy(result.buf, buf);
    return result;
}

int String::indexOf(char c) const {
    const char *p = strchr(buf, c);
    return p == nullptr ? -1 : p - buf;
}

int String::indexOf(const String &s) const {
    const char *p = strstr(buf, s.buf);
    return p == nullptr ? -1 : p - buf;
}

// ===================== comparison =====================

bool String::operator==(const String &s) const { return strcmp(buf, s.buf) == 0; }
bool String::operator!=(const String &s) const { return strcmp(buf, s.buf) != 0; }
bool String::operator>(const String &s) const  { return strcmp(buf, s.buf) > 0; }
bool String::operator<(const String &s) const  { return strcmp(buf, s.buf) < 0; }
bool String::operator<=(const String &s) const { return strcmp(buf, s.buf) <= 0; }
bool String::operator>=(const String &s) const { return strcmp(buf, s.buf) >= 0; }

// ===================== concatenation =====================

String String::operator+(const String &s) const {
    String result(size() + s.size());
    strcpy(result.buf, buf);
    strcat(result.buf, s.buf);
    return result;
}

String &String::operator+=(const String &s) {
    String result(size() + s.size());
    strcpy(result.buf, buf);
    strcat(result.buf, s.buf);
    swap(result);          // result's destructor frees our old buf
    return *this;
}

// ===================== I/O =====================

void String::print(ostream &out) const {
    out << buf;
}

void String::read(istream &in) {
    char temp[1024];
    in >> setw(sizeof temp) >> temp;   // setw prevents overflowing temp
    String t(temp);                    // exact-size copy
    swap(t);                           // t's destructor frees our old buf
}

ostream &operator<<(ostream &out, const String &s) {
    s.print(out);
    return out;
}

istream &operator>>(istream &in, String &s) {
    s.read(in);
    return in;
}
