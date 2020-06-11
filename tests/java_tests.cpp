#include <iostream>
#include "examples/java/preprocessor.hpp"


using namespace java;


static void test()
{
    std::vector<error> errors;

    const java_string input = u"123\n456\r789\r\n\\\\u2126=\\u2126\n101112";
    std::vector<java_string> lines = preprocess(input, errors);
    int x = 0;
}


void run_java_tests()
{
    std::cout << "running Java tests...\n";

    size_t test_count = 0;
    size_t success_count = 0;

    test();

    std::cout << "Java tests finished.\n";
    std::cout << "Tests run: " << test_count << '\n';
    std::cout << "Tests succeeded: " << success_count << '\n';
    std::cout << "Tests failed: " << (test_count - success_count) << '\n';
}
