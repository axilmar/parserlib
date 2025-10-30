#include "parserlib.hpp"


using namespace parserlib;


void run_tests() {
    rule<> r1;
    rule<> r2(r1);
    //rule<> r3(std::move(r1));
    rule<> r4 = 'a';
    rule<> r5 = [](parse_context<>& pc) { return false; };
    rule<> r6;
    r6 = 'a';
    r6 = r1;
    *r1;
    +r1;
    -r1;
    &r1;
    !r1;
}
