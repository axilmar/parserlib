#include "parserlib.hpp"


using namespace parserlib;


void run_tests() {
    std::string str = "abc";
    parse_context<> pc1(str);

    const auto grammar = terminal('a') ->* 1;
    grammar.parse(pc1);
}
