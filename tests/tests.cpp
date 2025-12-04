#include <iostream>
#include "parserlib.hpp"


using namespace parserlib;


void run_tests() {
    const auto t1 = terminal('a');
    const auto t2 = terminal("aa");
    const auto t3 = set("aaa");
    const auto t4 = range('0', '9');

    const auto u1 = *terminal('a');
    const auto u2 = +terminal('a');
    const auto u3 = -terminal('a');
    const auto u4 = &terminal('a');
    const auto u5 = !terminal('a');

    const auto s1 = terminal('a') >> 'b' >> 'c';
    const auto s2 = terminal('d') >> s1;
    const auto s3 = s1 >> terminal('d');

    const auto c1 = terminal('a') | 'b' | 'c';
    const auto c2 = terminal('d') | c1;
    const auto c3 = c1 | terminal('d');

    std::string src = "123";
    parse_context<> pc(src);
}
