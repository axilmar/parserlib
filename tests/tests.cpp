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

    parser<>::error(0, parser<>::skip_before(parser<>::terminal('a')));
    parser<>::error(0, parser<>::skip_after(parser<>::terminal('a')));

    parser<>::rule r1;
    parser<>::rule r2 = parser<>::terminal('a');
    parser<>::rule r3 = 'a';
    parser<>::rule r4 = "aaa";
    parser<>::rule r5 = 'a' >> r5;
    parser<>::rule r6 = "aaa" >> r5;
    parser<>::rule r7 = r7 >> 'a';
    parser<>::rule r8;
    r8 = 'a';
    r8 = "aaa";
    r8 = r7;
    r8 = r7 >> 'a';
    r8 = 'a' >> r7;

    parser<>::rule r9 = 'a' - r9;
    parser<>::rule r10 = r10 - 'a';

    parser<>::rule r11 = 'a' | r11;
    parser<>::rule r12 = "aaa" | r12;
    parser<>::rule r13 = r13 | 'a';
    parser<>::rule r14;
    r14 = r7 | 'a';
    r14 = 'a' | r7;

    parser<>::parse_node_ptr p1;
}
