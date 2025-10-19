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

    parser('a') >> 'b';
    'a' >> parser('b');
    parser('a') >> parser('b');
    parser('a') >> 'b' >> 'c';
    (parser('a') >> 'b') >> 'c';
    'a' >> (parser('b') >> 'c');
    (parser('a') >> 'b') >> (parser('c') >> 'd');

    parser('a') | 'b';
    'a' | parser('b');
    parser('a') | parser('b');
    parser('a') | 'b' | 'c';
    (parser('a') | 'b') | 'c';
    'a' | (parser('b') | 'c');
    (parser('a') | 'b') | (parser('c') | 'd');

    return 0;
}
