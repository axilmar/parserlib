#include "parserlib.hpp"


using namespace parserlib;


void run_tests() {
    std::string str = "abc";

    parse_context<> pc1(str);
    parse_context<std::string, int, text_position, case_insensitive_symbol_comparator> pc2(str);

    auto grammar = !&-+terminal('a');
    grammar.parse(pc1);
}
