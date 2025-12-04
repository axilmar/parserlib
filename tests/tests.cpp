#include <iostream>
#include "parserlib.hpp"


using namespace parserlib;


void run_tests() {
    auto t1 = terminal('a');
    auto t2 = terminal("aa");
    auto t3 = set("aaa");
    auto t4 = range('0', '9');

    auto u1 = *terminal('a');
    auto u2 = +terminal('a');
    auto u3 = -terminal('a');
    auto u4 = &terminal('a');
    auto u5 = !terminal('a');

    auto s1 = terminal('a') >> 'b' >> 'c';
    auto s2 = terminal('d') >> s1;
    auto s3 = s1 >> terminal('d');

    auto c1 = terminal('a') | 'b' | 'c';
    auto c2 = terminal('d') | c1;
    auto c3 = c1 | terminal('d') >> false;

    auto m1 = terminal('a')->*1;
    auto m2 = match_start() >> terminal('a') >> match_end(1);

    rule r1 = 'a';
    rule r2 = r1;
    rule r3 = {"r3", 'a'};
    rule r4 = {"r4", r3};
    r1 = "aaa";
    r2 = r1;

    std::string src = "123";
    parse_context<> pc(src);
}
