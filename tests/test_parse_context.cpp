#include <cassert>
#include <sstream>
#include "parserlib.hpp"


using namespace parserlib;


void test_parse_context() {
    using Stream = std::stringstream;
    using StreamContainer = stream_container<Stream>;
    using ParseDefinitions = parse_definitions<StreamContainer>;
    using ParseContext = parse_context<ParseDefinitions>;

    const auto grammar = +(range('0', '9'));

    {
        std::stringstream source;
        source << "0123456789";
        StreamContainer container(source);
        ParseContext context(container);
        assert(grammar.parse(context));
        assert(context.is_end_parse_position());
    }
}
