#include "parserlib.hpp"


using namespace parserlib;


class calculator_lexer_grammar {
public:
    enum class match_id_type {
        NUMBER,
        ADD,
        SUB,
        MUL,
        DIV,
        LEFT_PARENTHESIS,
        RIGHT_PARENTHESIS
    };

    enum class error_id_type {
        INVALID_TOKEN,
        INCOMPLETE_PARSE
    };

    using comparator_type = case_sensitive_comparator;

    template <class ParseContext>
    auto parse(ParseContext& pc) const noexcept {
        const auto whitespace = terminal(' ');
        const auto digit = range('0', '9');
        const auto number = (+digit >> -('.' >> +digit))->*match_id_type::NUMBER;
        const auto add = terminal('+')->*match_id_type::ADD;
        const auto sub = terminal('-')->*match_id_type::SUB;
        const auto mul = terminal('*')->*match_id_type::MUL;
        const auto div = terminal('/')->*match_id_type::DIV;
        const auto left_parenthesis = terminal('(')->*match_id_type::LEFT_PARENTHESIS;
        const auto right_parenthesis = terminal(')')->*match_id_type::RIGHT_PARENTHESIS;

        const auto token
            = number
            | add
            | sub
            | mul
            | div
            | left_parenthesis
            | right_parenthesis;

        const auto token_error = error(error_id_type::INVALID_TOKEN, skip_until(whitespace | token));

        const auto token1 = token | token_error;

        const auto grammar = *(whitespace | token1);

        return grammar.parse(pc);
    }

    static error_id_type incomplete_parse_error_id() noexcept {
        return error_id_type::INCOMPLETE_PARSE;
    }
};


using source_type = line_counting_string<>;
using lexer_type = lexer<calculator_lexer_grammar, source_type>;


static void test_tokenization() {
    {
        source_type source = "1@";
        auto result = lexer_type::parse(source);
    }
}


static void test_parsing() {
}


void test_parser() {
    test_tokenization();
    test_parsing();
}