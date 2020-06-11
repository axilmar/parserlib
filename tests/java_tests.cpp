#include <iostream>
#include "examples/java/tokenizer.hpp"


using namespace java;


static void test()
{
    std::vector<error> errors;

    const java_string input = u"0 123 123_456";
    std::vector<token> tokens = tokenize(input, errors);
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
