#include "parserlib.hpp"


using namespace parserlib;


void run_tests() {
    parser<>::terminal('a');
    parser<>::terminal("abc");
    parser<>::set("abc");
    parser<>::range('0', '9');

    *parser<>::terminal('a');
    +parser<>::terminal('a');
    -parser<>::terminal('a');
    &parser<>::terminal('a');
    !parser<>::terminal('a');

    parser<>::terminal('a') >> 'b' >> "aaa";
    parser<>::terminal('a') - 'b';
    parser<>::terminal('a') | 'b' | "aaa";

    parser<>::terminal('a') ->* 0;

    parser<>::any();
    parser<>::end();
    parser<>::false_();
    parser<>::true_();
    parser<>::newline(parser<>::terminal('a'));

    std::string s;
    parser<text_iterator<>>::parse_context pc(s);
    pc.increment_line();

    parser<>::function([](parser<>::parse_context& pc) { return true; });
}
