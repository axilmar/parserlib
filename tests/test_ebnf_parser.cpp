#include <cassert>
#include "languages/ebnf_parser.hpp"


using namespace parserlib;


static void test_tokenization() {
    using source_type = line_counting_string<>;

    source_type source =
        "\n"
        "\n"
        "\n"
        "\n"
        "(* comment *)\n"
        "<asv123_-bgh> hghgh123\n"
        "'a' '\\n'\n"
        "\"\\t\\n\\r\\\"\\\'abcd\"\n"
        "12345\n"
        "..\n"
        "(\n"
        ")\n"
        "[\n"
        "]\n"
        "{\n"
        "}\n"
        "?\n"
        "*\n"
        "+\n"
        ",\n"
        "|\n"
        "-\n"
        "::= = :\n"
        "#\n"
        "; .\n"
        "%0123456789%\n"
        ;

    const auto result = lexer<source_type, ebnf_parser::lexer_grammar>::parse(source);

    assert(result.success);
    assert(result.parsed_tokens.size() == 27);
    assert(result.parsed_tokens[0].id() == ebnf_parser::lexer_grammar::match_id_type::NEWLINE_TERMINATOR);
    assert(result.parsed_tokens[1].id() == ebnf_parser::lexer_grammar::match_id_type::NEWLINE_TERMINATOR);
    assert(result.parsed_tokens[2].id() == ebnf_parser::lexer_grammar::match_id_type::IDENTIFIER);
    assert(result.parsed_tokens[3].id() == ebnf_parser::lexer_grammar::match_id_type::IDENTIFIER);
    assert(result.parsed_tokens[4].id() == ebnf_parser::lexer_grammar::match_id_type::SINGLE_CHARACTER_STRING);
    assert(result.parsed_tokens[5].id() == ebnf_parser::lexer_grammar::match_id_type::SINGLE_CHARACTER_STRING);
    assert(result.parsed_tokens[6].id() == ebnf_parser::lexer_grammar::match_id_type::STRING);
    assert(result.parsed_tokens[7].id() == ebnf_parser::lexer_grammar::match_id_type::RANGE_OPERATOR);
    assert(result.parsed_tokens[8].id() == ebnf_parser::lexer_grammar::match_id_type::GROUP_START);
    assert(result.parsed_tokens[9].id() == ebnf_parser::lexer_grammar::match_id_type::GROUP_END);
    assert(result.parsed_tokens[10].id() == ebnf_parser::lexer_grammar::match_id_type::OPTIONAL_START);
    assert(result.parsed_tokens[11].id() == ebnf_parser::lexer_grammar::match_id_type::OPTIONAL_END);
    assert(result.parsed_tokens[12].id() == ebnf_parser::lexer_grammar::match_id_type::REPETITION_START);
    assert(result.parsed_tokens[13].id() == ebnf_parser::lexer_grammar::match_id_type::REPETITION_END);
    assert(result.parsed_tokens[14].id() == ebnf_parser::lexer_grammar::match_id_type::OPTIONAL_OPERATOR);
    assert(result.parsed_tokens[15].id() == ebnf_parser::lexer_grammar::match_id_type::ZERO_OR_MORE_OPERATOR);
    assert(result.parsed_tokens[16].id() == ebnf_parser::lexer_grammar::match_id_type::ONE_OR_MORE_OPERATOR);
    assert(result.parsed_tokens[17].id() == ebnf_parser::lexer_grammar::match_id_type::SEQUENCE_OPERATOR);
    assert(result.parsed_tokens[18].id() == ebnf_parser::lexer_grammar::match_id_type::CHOICE_OPERATOR);
    assert(result.parsed_tokens[19].id() == ebnf_parser::lexer_grammar::match_id_type::EXCEPTION_OPERATOR);
    assert(result.parsed_tokens[20].id() == ebnf_parser::lexer_grammar::match_id_type::DEFINITION_OPERATOR);
    assert(result.parsed_tokens[21].id() == ebnf_parser::lexer_grammar::match_id_type::DEFINITION_OPERATOR);
    assert(result.parsed_tokens[22].id() == ebnf_parser::lexer_grammar::match_id_type::DEFINITION_OPERATOR);
    assert(result.parsed_tokens[23].id() == ebnf_parser::lexer_grammar::match_id_type::LEXER_OPERATOR);
    assert(result.parsed_tokens[24].id() == ebnf_parser::lexer_grammar::match_id_type::TERMINATOR);
    assert(result.parsed_tokens[25].id() == ebnf_parser::lexer_grammar::match_id_type::TERMINATOR);
    assert(result.parsed_tokens[26].id() == ebnf_parser::lexer_grammar::match_id_type::STRING_SET);
}


void test_ebnf_parser() {
    test_tokenization();
}
