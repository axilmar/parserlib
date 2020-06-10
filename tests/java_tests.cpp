#include "examples/java/character_parser.hpp"
#include "examples/java/tokenizer.hpp"


using namespace java;


void run_java_tests()
{
    std::u16string input = u"\\\\2126 \\u2126 \\u212 \\uu2126";
    std::vector<Error> errors;
    java_string s = parse_characters(input, errors);
    std::vector<token> t = tokenize(s, errors);
}
