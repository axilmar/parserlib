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

    rule<> r1;
    rule<> r2 = 'a';
    rule<> r3 = "aaa";
    rule<> r4 = set("abc");
    rule<> r5 = range('a', 'z');
    rule<> r6 = r2 >> r3;
    rule<> r7 = r2 | r3;
    rule<> r8 = r1;
    rule<> r9 = r9 >> r1;

    r2 = 'a';
    r3 = "aaa";
    r4 = set("abc");
    r5 = range('a', 'z');
    r6 = r2 >> r3;
    r7 = r2 | r3;
    r8 = r1;

    parser('a') ->* 1;

    return 0;
}
