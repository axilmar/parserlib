#include <cassert>
#include <iostream>
#include "languages/ebnf_parser.hpp"


using namespace parserlib;


static void test_tokenization() {
    std::string source =
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
        "#lexerid\n"
        "; .\n"
        "%0123456789%\n"
        ;

    const auto result = lexer<std::string, ebnf_parser::lexer_grammar>::parse(source);

    assert(result.success);
    assert(result.parsed_tokens.size() == 28);
    assert(result.parsed_tokens[0].id() == ebnf_parser::lexer_grammar::match_id_type::NEWLINE_TERMINATOR);
    assert(result.parsed_tokens[1].id() == ebnf_parser::lexer_grammar::match_id_type::NEWLINE_TERMINATOR);
    assert(result.parsed_tokens[2].id() == ebnf_parser::lexer_grammar::match_id_type::PARSER_IDENTIFIER);
    assert(result.parsed_tokens[3].id() == ebnf_parser::lexer_grammar::match_id_type::PARSER_IDENTIFIER);
    assert(result.parsed_tokens[4].id() == ebnf_parser::lexer_grammar::match_id_type::SINGLE_CHARACTER_STRING);
    assert(result.parsed_tokens[5].id() == ebnf_parser::lexer_grammar::match_id_type::SINGLE_CHARACTER_STRING);
    assert(result.parsed_tokens[6].id() == ebnf_parser::lexer_grammar::match_id_type::STRING);
    assert(result.parsed_tokens[7].id() == ebnf_parser::lexer_grammar::match_id_type::INTEGER);
    assert(result.parsed_tokens[8].id() == ebnf_parser::lexer_grammar::match_id_type::RANGE_OPERATOR);
    assert(result.parsed_tokens[9].id() == ebnf_parser::lexer_grammar::match_id_type::GROUP_START);
    assert(result.parsed_tokens[10].id() == ebnf_parser::lexer_grammar::match_id_type::GROUP_END);
    assert(result.parsed_tokens[11].id() == ebnf_parser::lexer_grammar::match_id_type::OPTIONAL_START);
    assert(result.parsed_tokens[12].id() == ebnf_parser::lexer_grammar::match_id_type::OPTIONAL_END);
    assert(result.parsed_tokens[13].id() == ebnf_parser::lexer_grammar::match_id_type::REPETITION_START);
    assert(result.parsed_tokens[14].id() == ebnf_parser::lexer_grammar::match_id_type::REPETITION_END);
    assert(result.parsed_tokens[15].id() == ebnf_parser::lexer_grammar::match_id_type::OPTIONAL_OPERATOR);
    assert(result.parsed_tokens[16].id() == ebnf_parser::lexer_grammar::match_id_type::LOOP_0_OPERATOR);
    assert(result.parsed_tokens[17].id() == ebnf_parser::lexer_grammar::match_id_type::LOOP_1_OPERATOR);
    assert(result.parsed_tokens[18].id() == ebnf_parser::lexer_grammar::match_id_type::CONCATENATION_OPERATOR);
    assert(result.parsed_tokens[19].id() == ebnf_parser::lexer_grammar::match_id_type::ALTERNATION_OPERATOR);
    assert(result.parsed_tokens[20].id() == ebnf_parser::lexer_grammar::match_id_type::EXCLUSION_OPERATOR);
    assert(result.parsed_tokens[21].id() == ebnf_parser::lexer_grammar::match_id_type::DEFINITION_OPERATOR);
    assert(result.parsed_tokens[22].id() == ebnf_parser::lexer_grammar::match_id_type::DEFINITION_OPERATOR);
    assert(result.parsed_tokens[23].id() == ebnf_parser::lexer_grammar::match_id_type::DEFINITION_OPERATOR);
    assert(result.parsed_tokens[24].id() == ebnf_parser::lexer_grammar::match_id_type::LEXER_IDENTIFIER);
    assert(result.parsed_tokens[25].id() == ebnf_parser::lexer_grammar::match_id_type::TERMINATOR);
    assert(result.parsed_tokens[26].id() == ebnf_parser::lexer_grammar::match_id_type::TERMINATOR);
    assert(result.parsed_tokens[27].id() == ebnf_parser::lexer_grammar::match_id_type::STRING_SET);
}


static void test_parsing() {
    std::string source =
        "rule1 = rule2 rule3\n"
        "      | rule4 rule5;\n"
        "\n"
        "rule6 = rule7?;\n"
        "\n"
        "rule7 = rule8*;\n"
        "\n"
        "rule8 = rule9+;\n"
        "\n"
        "rule10 = rule11 - rule12;\n"
        "\n"
        "rule13 = 3 * rule14;\n"
        "\n"
        "rule15 = 'a' .. 'b';\n"
        "\n"
        "rule16 = [rule17];\n"
        "\n"
        "rule18 = {rule19};\n"
        "\n"
        "#rule20 = (rule21 rule22);\n"
        "\n"
        "#rule23 = #rule24 <rule25> #<rule26>;\n"
        "\n"
        "#rule27 = %123%;\n"
        "\n"
        "#rule28 = 'abc'\n"
        "";

    const auto result = parser<std::string, ebnf_parser::lexer_grammar, ebnf_parser::parser_grammar>::parse(source);

    for (const auto& ast_node : result.ast_nodes) {
        ast_node->print(std::cout);
    }

    printf("1");
}


void test_ebnf_parser() {
    test_tokenization();
    test_parsing();
}
