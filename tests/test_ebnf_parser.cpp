#include <cassert>
#include <iostream>
#include "languages/ebnf_parser.hpp"


using namespace parserlib;


void test_ebnf_parser() {
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
}
