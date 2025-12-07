#include <iostream>
#include <vector>
#include <list>
#include "parserlib.hpp"


using namespace parserlib;


void run_tests() {
    std::vector s1{1, 2, 3, 4, 5};
    parse_context pc1(s1);

    generic_source s2(s1);
    parse_context pc2(s2);

    std::list s3{1, 2, 3, 4, 5};
    parse_context pc3(s3);

    generic_source s4({ 1, 2, 3, 4, 5 });
    parse_context pc4(s4);

    generic_source s5("12345");
    parse_context pc5(s5);

    generic_source s6 = "12345";
    parse_context pc6(s6);

    range(1, 2);
    range('0', '9');
    set(std::string("12345"));
    set("1234");
    set(std::vector{1, 2, 3, 4});
    set({ 1, 2, 3, 4 });

    rule r1;
    rule r2 = r1;
    rule r3;
    r3 = r1;

    const auto s = terminal('a') >> 'b' >> 'c' >> r1;
    const auto c = terminal('a') | 'b' | 'c' | r1;
}

/*
TODO:
ast
debug
error
function
id_name
named
skip_after
skip_before
*/