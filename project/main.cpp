#include "parserlib.hpp"


using namespace parserlib;


int main() {
    std::string str = "the quick brown fox";
    parse_context pc(str);

    *parser('a');
    *parser("aa");
    *set("ab");
    *range('a', 'z');

    +parser('a');
    +parser("aa");
    +set("ab");
    +range('a', 'z');

    -parser('a');
    -parser("aa");
    -set("ab");
    -range('a', 'z');

    &parser('a');
    &parser("aa");
    &set("ab");
    &range('a', 'z');

    !parser('a');
    !parser("aa");
    !set("ab");
    !range('a', 'z');

    return 0;
}
