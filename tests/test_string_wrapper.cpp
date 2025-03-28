#include <cassert>
#include <algorithm>
#include <sstream>
#include <ios>
#include "parserlib.hpp"


using namespace parserlib;


static void test_string_wrapper_only() {
    const int DIGIT = 1;
    const int NEWLINE = 2;

    const auto digit = range('0', '9')->*DIGIT;
    const auto line_end = newline("\r\n")->*NEWLINE;
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

    size_t line = 1;
    size_t col = 1;
    for (size_t i = 0, m = 0; i < source.string().size(); ++i, ++m) {
        const char c = source.string()[i];
        assert(context.matches()[m].span().begin().line() == line);
        assert(context.matches()[m].span().begin().column() == col);
        if (c == '\r') {
            ++line;
            col = 1;
            ++i;
        }
        else {
            ++col;
        }

    }
}


static void test_string_wrapper_over_stream_wrapper() {
    const int DIGIT = 1;
    const int NEWLINE = 2;

    const auto digit = range('0', '9')->*DIGIT;
    const auto line_end = newline("\r\n")->*NEWLINE;
    const auto grammar = *(digit | line_end);

    using StreamWrapper = stream_wrapper<std::stringstream>;
    using StringWrapper = string_wrapper<StreamWrapper>;
    using ParseDefinitions = parse_definitions<StringWrapper>;
    using ParseContext = parse_context<ParseDefinitions>;

    std::string inputString = "123\r\n456\r\n789";
    std::stringstream inputStream;
    inputStream << inputString;
    StreamWrapper streamSource(inputStream);
    StringWrapper source(streamSource);

    ParseContext context(source);

    assert(grammar.parse(context));
    assert(context.is_end_parse_position());

    assert(context.matches().size() == 11);

    size_t line = 1;
    size_t col = 1;
    for (size_t i = 0, m = 0; i < inputString.size(); ++i, ++m) {
        const char c = inputString[i];
        assert(context.matches()[m].span().begin().line() == line);
        assert(context.matches()[m].span().begin().column() == col);
        if (c == '\r') {
            ++line;
            col = 1;
            ++i;
        }
        else {
            ++col;
        }

    }
}


void test_string_wrapper() {
    test_string_wrapper_only();
    test_string_wrapper_over_stream_wrapper();
}
