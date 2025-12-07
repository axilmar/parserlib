#include <iostream>
#include <vector>
#include <list>
#include "parserlib.hpp"


using namespace parserlib;


void run_tests() {
    std::vector s1{1, 2, 3, 4, 5};
    parse_context pc1(s1);

    generic_source s2{ s1 };
    parse_context pc2(s2);

    std::list s3{1, 2, 3, 4, 5};
    parse_context pc3(s3);

    range(1, 2);
    range('0', '9');
    set(std::string("12345"));
    set("1234");
    set(std::vector{1, 2, 3, 4});
    set({1, 2, 3, 4});
}
