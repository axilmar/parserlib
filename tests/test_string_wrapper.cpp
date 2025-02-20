#include <cassert>
#include <algorithm>
#include "parserlib.hpp"


using namespace parserlib;


void test_string_wrapper() {
    const int DIGIT = 1;
    const int NEWLINE = 2;

    const auto digit = range('0', '9') ->* DIGIT;
    const auto line_end = newline("\r\n") ->* NEWLINE;
    const auto grammar = *(digit | line_end);

    using StringWrapper = string_wrapper<std::string>;
    using ParseDefinitions = parse_definitions<StringWrapper>;
    using ParseContext = parse_context<ParseDefinitions>;

    std::string input = "123\r\n456\r\n789";
    StringWrapper source(input);

    ParseContext context(source);

    assert(grammar.parse(context));
    assert(context.is_end_parse_position());

    assert(context.matches().size() == 11);

    size_t line = 0;
    size_t col = 0;
    for (size_t i = 0, m = 0; i < source.string().size(); ++i, ++m) {
        const char c = source.string()[i];
        assert(context.matches()[m].span().begin().line() == line);
        assert(context.matches()[m].span().begin().column() == col);
        if (c == '\r') {
            ++line;
            col = 0;
            ++i;
        }
        else {
            ++col;
        }

    }
}

