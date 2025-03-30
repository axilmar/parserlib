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


static void test_utf8_string() {
    {
        using ParseContext = parse_context<utf8_string<>, int, int, case_sensitive_comparator>;
        utf8_string<> input = "\x69\x64\x65\x6E\x74\x69\x66\x69\x65\x72\x31\x3B";
        ParseContext context(input);
        assert(grammar.parse(context));
    }

    {
        using ParseContext = parse_context<utf8_string<>, int, int, case_sensitive_comparator>;
        utf8_string<> input = "\xCE\xB1\xCE\xB9\xCE\xBD\xCF\x84\xCE\xB5\xCE\xBD\xCF\x84\xCE\xB9\xCF\x86\xCE\xB1\xCE\xB9\xCE\xB5\xCF\x81\x3B";
        ParseContext context(input);
        assert(grammar.parse(context));
    }
}


static void test_line_counting_string() {
    line_counting_string<> str = 
        "1a\n"
        "2\n"
        "\n"
        "4\n";

    auto it = str.begin();
    assert(*it == '1' && it.line() == 1 && it.column() == 1);

    ++it;
    assert(*it == 'a' && it.line() == 1 && it.column() == 2);

    ++it;
    assert(*it == '\n' && it.line() == 1 && it.column() == 3);

    ++it;
    assert(*it == '2' && it.line() == 2 && it.column() == 1);

    ++it;
    assert(*it == '\n' && it.line() == 2 && it.column() == 2);

    ++it;
    assert(*it == '\n' && it.line() == 3 && it.column() == 1);

    ++it;
    assert(*it == '4' && it.line() == 4 && it.column() == 1);

    ++it;
    assert(*it == '\n' && it.line() == 4 && it.column() == 2);

    ++it;
    assert(it == str.end());
}


void test_strings() {
    test_utf8_string();
    test_line_counting_string();
}

