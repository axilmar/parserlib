#include <cassert>
#include "parserlib.hpp"


using namespace parserlib;


static void test_symbol() {
    const auto grammar = terminal('a');

    {
        std::string source = "a";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.is_end_parse_position());
    }
}


void run_tests() {
    test_symbol();
}
