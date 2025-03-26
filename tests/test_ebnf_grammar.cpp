#include "parserlib/string_wrapper.hpp"
#include "parserlib/ebnf_grammar.hpp"
#include "parserlib/compiler_frontend.hpp"


using namespace parserlib;


static void test_lexer() {
    using cf = compiler_frontend<ebnf_grammar>;

    std::string source =
        "(* lexer identifier\n *)\n"
        "FOO\n"
        "\n"
        "(* parser identifier *)\n"
        "foo\n"
        "\n"
        "(* string *)\n"
        "\" \\\' \\\" \\n \\t \\v \\r \\0 \\f \\xA \\XAB \\xABC \\x1234 the quick brown fox \"\n"
        "\n"
        "@\n"
        "(* other *)\n"
        "=(){}[]*+?|-;\n"
        ;
    string_wrapper<> input(source);
    const auto result = cf::lexer::tokenize(input);

    assert(result.tokens[0].id() == ebnf_grammar::lexer::TOKEN_ID::LEXER_IDENTIFIER);
    assert(result.tokens[1].id() == ebnf_grammar::lexer::TOKEN_ID::PARSER_IDENTIFIER);
    assert(result.tokens[2].id() == ebnf_grammar::lexer::TOKEN_ID::STRING);
    assert(result.tokens[3].id() == ebnf_grammar::lexer::TOKEN_ID::ASSIGNMENT);
    assert(result.tokens[4].id() == ebnf_grammar::lexer::TOKEN_ID::GROUP_START);
    assert(result.tokens[5].id() == ebnf_grammar::lexer::TOKEN_ID::GROUP_END);
    assert(result.tokens[6].id() == ebnf_grammar::lexer::TOKEN_ID::REPETITION_START);
    assert(result.tokens[7].id() == ebnf_grammar::lexer::TOKEN_ID::REPETITION_END);
    assert(result.tokens[8].id() == ebnf_grammar::lexer::TOKEN_ID::OPTIONAL_START);
    assert(result.tokens[9].id() == ebnf_grammar::lexer::TOKEN_ID::OPTIONAL_END);
    assert(result.tokens[10].id() == ebnf_grammar::lexer::TOKEN_ID::LOOP_0);
    assert(result.tokens[11].id() == ebnf_grammar::lexer::TOKEN_ID::LOOP_1);
    assert(result.tokens[12].id() == ebnf_grammar::lexer::TOKEN_ID::OPTIONAL);
    assert(result.tokens[13].id() == ebnf_grammar::lexer::TOKEN_ID::BRANCH);
    assert(result.tokens[14].id() == ebnf_grammar::lexer::TOKEN_ID::EXCLUSION);
    assert(result.tokens[15].id() == ebnf_grammar::lexer::TOKEN_ID::TERMINATOR);

    assert(result.errors.size() == 1);
    assert(result.errors[0].source() == "@");
}


static void test_parser() {
    using cf = compiler_frontend<ebnf_grammar>;

    std::string source =
        "DIGIT = '0' .. '9';\n"
        "LETTER = 'A' .. 'Z' | 'a' .. 'z';\n"
        "IDENTIFIER = LETTER (LETTER | DIGIT | '-');\n"
        "\n"
        "INTEGER = DIGIT+;\n"
        "value = INTEGER;\n"
        "struct_member = \"var\" IDENTIFIER '=' value ';';\n"
        "struct_declaration = \"struct\" IDENTIFIER \"{\" struct_member* \"}\";\n"
        ;

    string_wrapper<> input(source);

    const auto result = cf::parse(input);

    assert(result.success);
    assert(result.parser.ast_nodes.size() == 7);
    assert(result.parser.ast_nodes[0]->id() == ebnf_grammar::parser::AST_ID::LEXER_DECLARATION);
    assert(result.parser.ast_nodes[1]->id() == ebnf_grammar::parser::AST_ID::LEXER_DECLARATION);
    assert(result.parser.ast_nodes[2]->id() == ebnf_grammar::parser::AST_ID::LEXER_DECLARATION);
    assert(result.parser.ast_nodes[3]->id() == ebnf_grammar::parser::AST_ID::LEXER_DECLARATION);
    assert(result.parser.ast_nodes[4]->id() == ebnf_grammar::parser::AST_ID::PARSER_DECLARATION);
    assert(result.parser.ast_nodes[5]->id() == ebnf_grammar::parser::AST_ID::PARSER_DECLARATION);
    assert(result.parser.ast_nodes[6]->id() == ebnf_grammar::parser::AST_ID::PARSER_DECLARATION);
}


void test_ebnf_grammar() {
    test_lexer();
    test_parser();
}
