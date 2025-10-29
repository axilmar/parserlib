#include "parserlib.hpp"


using namespace parserlib;


void run_tests() {
    std::string str = "abc";

    parse_context<> pc1(str);
    parse_context<std::string, int, text_position, case_insensitive_symbol_comparator> pc2(str);

    //auto grammar1 = !&-+terminal('a');
    //grammar1.parse(pc1);

    const auto grammar2 = terminal('a') | (terminal('b') | 'c');
    grammar2.parse(pc1);
}
