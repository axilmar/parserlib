#include <cassert>
#include "parserlib/lexer.hpp"


//token type
enum class token_type {
    a,
    b,
    c
};


//lexer type
using lexer_type = parserlib::lexer<std::string, token_type>;


//parse engine type
using pe = lexer_type::parse_engine_type;


//white space
static auto ws = pe::range('\0', ' ');


//token 'a'
static auto token_a = +pe::terminal('a') ->* token_type::a;


//token 'b'
static auto token_b = +pe::terminal('b') ->* token_type::b;


//token 'c'
static auto token_c = +pe::terminal('c') ->* token_type::c;


//lexer grammar
static auto lexer_grammar = *(ws | token_a | token_b | token_c);


void test_lexer() {
    std::string input = "a b c\naa ccccc\nbbb";
    auto [success, tokens, it] = lexer_type::parse(input, lexer_grammar);

    assert(tokens.size() == 6);

    assert(tokens[0].get_start_position().get_iterator() == input.begin());
    assert(tokens[0].get_start_position().get_line() == 1);
    assert(tokens[0].get_start_position().get_column() == 1);
    assert(tokens[0].get_end_position().get_iterator() == std::next(input.begin(), 1));
    assert(tokens[0].get_end_position().get_line() == 1);
    assert(tokens[0].get_end_position().get_column() == 2);
    assert(tokens[0].get_source() == "a");

    assert(tokens[1].get_start_position().get_iterator() == std::next(input.begin(), 2));
    assert(tokens[1].get_start_position().get_line() == 1);
    assert(tokens[1].get_start_position().get_column() == 3);
    assert(tokens[1].get_end_position().get_iterator() == std::next(input.begin(), 3));
    assert(tokens[1].get_end_position().get_line() == 1);
    assert(tokens[1].get_end_position().get_column() == 4);
    assert(tokens[1].get_source() == "b");

    assert(tokens[2].get_start_position().get_iterator() == std::next(input.begin(), 4));
    assert(tokens[2].get_start_position().get_line() == 1);
    assert(tokens[2].get_start_position().get_column() == 5);
    assert(tokens[2].get_end_position().get_iterator() == std::next(input.begin(), 5));
    assert(tokens[2].get_end_position().get_line() == 1);
    assert(tokens[2].get_end_position().get_column() == 6);
    assert(tokens[2].get_source() == "c");

    assert(tokens[3].get_start_position().get_iterator() == std::next(input.begin(), 6));
    assert(tokens[3].get_start_position().get_line() == 2);
    assert(tokens[3].get_start_position().get_column() == 1);
    assert(tokens[3].get_end_position().get_iterator() == std::next(input.begin(), 8));
    assert(tokens[3].get_end_position().get_line() == 2);
    assert(tokens[3].get_end_position().get_column() == 3);
    assert(tokens[3].get_source() == "aa");

    assert(tokens[4].get_start_position().get_iterator() == std::next(input.begin(), 9));
    assert(tokens[4].get_start_position().get_line() == 2);
    assert(tokens[4].get_start_position().get_column() == 4);
    assert(tokens[4].get_end_position().get_iterator() == std::next(input.begin(), 14));
    assert(tokens[4].get_end_position().get_line() == 2);
    assert(tokens[4].get_end_position().get_column() == 9);
    assert(tokens[4].get_source() == "ccccc");

    assert(tokens[5].get_start_position().get_iterator() == std::next(input.begin(), 15));
    assert(tokens[5].get_start_position().get_line() == 3);
    assert(tokens[5].get_start_position().get_column() == 1);
    assert(tokens[5].get_end_position().get_iterator() == std::next(input.begin(), 18));
    assert(tokens[5].get_end_position().get_line() == 3);
    assert(tokens[5].get_end_position().get_column() == 4);
    assert(tokens[5].get_source() == "bbb");
}
