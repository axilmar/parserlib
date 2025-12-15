#include <cassert>
#include "parserlib.hpp"


using namespace parserlib;


void run_tests() {
    std::stringstream stream{"b"};
    istream_parse_context<> pc(stream);

    rule<istream_parse_context<>> grammar;

    const auto g = terminal('a') | grammar;

    grammar = g;

    const bool result = grammar.parse(pc);
}
