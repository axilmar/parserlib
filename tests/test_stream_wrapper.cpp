#include <sstream>
#include <iostream>
#include <cassert>
#include "parserlib.hpp"


using namespace parserlib;


static void test_stream_wrapper_iteration() {
    {
        std::stringstream stream1, stream2;
        stream1 << "12345";
        stream_wrapper<std::stringstream> container(stream1);
        for (auto it = container.begin(); it != container.end(); ++it) {
            stream2 << *it;
        }
        assert(stream1.str() == stream2.str());
    }
}


static void test_stream_wrapper_parsing() {
    const int DIGIT = 1;
    const int NEWLINE = 2;

    const auto digit = range('0', '9')->*DIGIT;
    const auto line_end = terminal("\r\n")->*NEWLINE;
    const auto grammar = *(digit | line_end);

    using StreamWrapper = stream_wrapper<std::stringstream>;
    using ParseDefinitions = parse_definitions<StreamWrapper>;
    using ParseContext = parse_context<ParseDefinitions>;

    std::string inputString = "123\r\n456\r\n789";
    std::stringstream inputStream;
    inputStream << inputString;
    StreamWrapper source(inputStream);

    ParseContext context(source);

    assert(grammar.parse(context));
    assert(context.is_end_parse_position());

    assert(context.matches().size() == 11);
}


void test_stream_wrapper() {
    test_stream_wrapper_iteration();
    test_stream_wrapper_parsing();
}
