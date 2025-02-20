#include <cassert>
#include <sstream>
#include "parserlib.hpp"


using namespace parserlib;


static void test_parse_stream() {
    using Stream = std::stringstream;
    using StreamContainer = stream_wrapper<Stream>;
    using ParseDefinitions = parse_definitions<StreamContainer>;
    using ParseContext = parse_context<ParseDefinitions>;
    const auto grammar = +(range('0', '9'));
    std::stringstream source;
    source << "0123456789";
    StreamContainer container(source);
    ParseContext context(container);
    assert(grammar.parse(context));
    assert(context.is_end_parse_position());
}


static void test_parse_case_insensitive() {
    {
        using ParseDefinitions = parse_definitions<std::string, int, int, case_insensitive_comparator>;
        using ParseContext = parse_context<ParseDefinitions>;
        const auto grammar = terminal("abc");
        std::string source = "AbC";
        ParseContext context(source);
        assert(grammar.parse(context));
        assert(context.is_end_parse_position());
    }

    {
        using ParseDefinitions = parse_definitions<std::string, int, int, case_insensitive_comparator>;
        using ParseContext = parse_context<ParseDefinitions>;
        const auto grammar = terminal("abc");
        std::string source = "Abd";
        ParseContext context(source);
        assert(!grammar.parse(context));
        assert(context.parse_position() == source.begin());
    }
}


void test_parse_context() {
    test_parse_stream();
    test_parse_case_insensitive();
}
