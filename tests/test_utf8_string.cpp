#include <cassert>
#include <cwctype>
#include "parserlib.hpp"


using namespace parserlib;


static bool test_isalpha(uint32_t v) {
    return std::iswalpha((wint_t)v);
}


static auto letter = function([](auto& context) {
    if (test_isalpha(*context.parse_position())) {
        context.increment_parse_position();
        return true;
    }
    return false;
    });


static auto digit = range('0', '9');


static auto identifier = letter >> *(letter | digit | '_');


static auto grammar = identifier >> ';';


void test_utf8_string() {
    {
        using ParseDefinitions = parse_definitions<utf8_string<>>;
        using ParseContext = parse_context<ParseDefinitions>;
        utf8_string<> input = "\x69\x64\x65\x6E\x74\x69\x66\x69\x65\x72\x31\x3B";
        ParseContext context(input);
        assert(grammar.parse(context));
    }

    {
        using ParseDefinitions = parse_definitions<utf8_string<>>;
        using ParseContext = parse_context<ParseDefinitions>;
        utf8_string<> input = "\xCE\xB1\xCE\xB9\xCE\xBD\xCF\x84\xCE\xB5\xCE\xBD\xCF\x84\xCE\xB9\xCF\x86\xCE\xB1\xCE\xB9\xCE\xB5\xCF\x81\x3B";
        ParseContext context(input);
        assert(grammar.parse(context));
    }
}
