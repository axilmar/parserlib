#include <cassert>
#include "parserlib.hpp"


using namespace parserlib;


void run_tests() {
    std::stringstream stream{"b"};
    istream_parse_context<> pc(stream);

    rule<istream_parse_context<>> grammar;

    const auto g = terminal('a')->*1 >> debug(any()) >> end() >> false >> newline('\n') >> error(1, skip_after('a'));

    grammar = g;

    const bool result = grammar.parse(pc);
}
