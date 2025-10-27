#include "parserlib/parse_context.hpp"


using namespace parserlib;


void run_tests() {
    std::string str = "abc";

    parse_context<> pc1(str);
    parse_context<std::string, int, text_position, case_insensitive_symbol_comparator> pc2(str);
}
