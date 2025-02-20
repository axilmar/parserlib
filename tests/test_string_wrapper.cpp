#include <cassert>
#include "parserlib.hpp"


using namespace parserlib;


static void test_string_wrapper_by_reference() {
    const int DIGIT = 1;
    const int NEWLINE = 2;

    const auto digit = range('0', '9') ->* DIGIT;
    const auto newline = terminal('\n') ->* NEWLINE;
    const auto grammar = *(digit | newline);

    using StringWrapper = string_wrapper<std::string>;
    using ParseDefinitions = parse_definitions<StringWrapper>;
    using ParseContext = parse_context<ParseDefinitions>;

    std::string input = "123\n456\n789";
    StringWrapper source(input);

    ParseContext context(source);

    assert(grammar.parse(context));
    assert(context.is_end_parse_position());

    assert(context.matches().size() == 11);

    size_t line = 0;
    size_t col = 0;
    for (size_t i = 0; i < source.string().size(); ++i) {
        const char c = source.string()[i];
        assert(context.matches()[i].span().begin().line() == line);
        assert(context.matches()[i].span().begin().column() == col);
        if (c != '\n') {
            ++col;
        }
        else {
            ++line;
            col = 0;
        }
    }
}


static void test_string_wrapper_by_value() {
    const int DIGIT = 1;
    const int NEWLINE = 2;

    const auto digit = range('0', '9')->*DIGIT;
    const auto newline = terminal('\n')->*NEWLINE;
    const auto grammar = *(digit | newline);

    using StringWrapper = string_wrapper<std::string, DefaultNewlineSequenceLength, std::string>;
    using ParseDefinitions = parse_definitions<StringWrapper>;
    using ParseContext = parse_context<ParseDefinitions>;

    StringWrapper source = "123\n456\n789";

    ParseContext context(source);

    assert(grammar.parse(context));
    assert(context.is_end_parse_position());

    assert(context.matches().size() == 11);

    size_t line = 0;
    size_t col = 0;
    for (size_t i = 0; i < source.string().size(); ++i) {
        const char c = source.string()[i];
        assert(context.matches()[i].span().begin().line() == line);
        assert(context.matches()[i].span().begin().column() == col);
        if (c != '\n') {
            ++col;
        }
        else {
            ++line;
            col = 0;
        }
    }
}


void test_string_wrapper() {
    test_string_wrapper_by_reference();
    test_string_wrapper_by_value();
}

