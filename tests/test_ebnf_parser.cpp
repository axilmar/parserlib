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
        "@\n"
        "?\n"
        "*\n"
        "+\n"
        ",\n"
        "|\n"
        "-\n"
        "::= = :\n"
        "#\n"
        "; .\n"
        "%\n"
        ;

    const auto result = lexer<source_type, ebnf_parser::lexer_grammar>::parse(source);

    printf("1");
}


void test_ebnf_parser() {
    test_tokenization();
}
