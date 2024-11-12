#include <cassert>
#include <cwctype>
#include "parserlib/parser_engine.hpp"
#include "parserlib/utf8_string.hpp"


using pe = parserlib::parser_engine<parserlib::utf8_string>;


bool test_isalpha(uint32_t v) {
    const bool result = std::iswalpha((wint_t)v);
    return result;
}


auto letter = pe::terminal(&test_isalpha);


auto digit = pe::terminal(&std::iswdigit);


auto identifier = letter >> *(letter | digit | '_');


auto grammar = identifier >> ';';


void test_utf8_string() {
    {
        parserlib::utf8_string input = "\x69\x64\x65\x6E\x74\x69\x66\x69\x65\x72\x31\x3B";
        auto [success, ast, it, errors] = pe::parse(input, grammar);
        assert(success);
    }

    {
        parserlib::utf8_string input = "\xCE\xB1\xCE\xB9\xCE\xBD\xCF\x84\xCE\xB5\xCE\xBD\xCF\x84\xCE\xB9\xCF\x86\xCE\xB1\xCE\xB9\xCE\xB5\xCF\x81\x3B";
        auto [success, ast, it, errors] = pe::parse(input, grammar);
        assert(success);
    }
}
