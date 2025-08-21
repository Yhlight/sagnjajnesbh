#include <iostream>
#include <cassert>
#include "utils/StringUtils.h"

using namespace chtl::utils;

void TestTrim() {
    assert(StringUtils::Trim("  hello  ") == "hello");
    assert(StringUtils::Trim("\t\nhello\r\n") == "hello");
    assert(StringUtils::Trim("hello") == "hello");
    assert(StringUtils::Trim("") == "");
    assert(StringUtils::Trim("   ") == "");
    
    std::cout << "TestTrim passed!" << std::endl;
}

void TestSplit() {
    auto result = StringUtils::Split("a,b,c", ',');
    assert(result.size() == 3);
    assert(result[0] == "a");
    assert(result[1] == "b");
    assert(result[2] == "c");
    
    result = StringUtils::Split("hello", ',');
    assert(result.size() == 1);
    assert(result[0] == "hello");
    
    std::cout << "TestSplit passed!" << std::endl;
}

void TestJoin() {
    std::vector<std::string> strings = {"a", "b", "c"};
    assert(StringUtils::Join(strings, ",") == "a,b,c");
    assert(StringUtils::Join(strings, " ") == "a b c");
    
    strings = {"hello"};
    assert(StringUtils::Join(strings, ",") == "hello");
    
    strings = {};
    assert(StringUtils::Join(strings, ",") == "");
    
    std::cout << "TestJoin passed!" << std::endl;
}

void TestStartsEndsWith() {
    assert(StringUtils::StartsWith("hello world", "hello"));
    assert(!StringUtils::StartsWith("hello world", "world"));
    assert(StringUtils::EndsWith("hello world", "world"));
    assert(!StringUtils::EndsWith("hello world", "hello"));
    
    std::cout << "TestStartsEndsWith passed!" << std::endl;
}

void TestReplace() {
    assert(StringUtils::Replace("hello world", "world", "universe") == "hello universe");
    assert(StringUtils::Replace("aaa", "a", "b") == "bbb");
    assert(StringUtils::Replace("hello", "x", "y") == "hello");
    
    std::cout << "TestReplace passed!" << std::endl;
}

int main() {
    std::cout << "Running StringUtils tests..." << std::endl;
    
    TestTrim();
    TestSplit();
    TestJoin();
    TestStartsEndsWith();
    TestReplace();
    
    std::cout << "All tests passed!" << std::endl;
    return 0;
}