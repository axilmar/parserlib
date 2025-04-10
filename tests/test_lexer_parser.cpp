#include <sstream>
#include <stdexcept>
#include "parserlib.hpp"


using namespace parserlib;


using source_type = line_counting_string<>;


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

    //using comparator_type = case_sensitive_comparator;

    //static constexpr error_id_type incomplete_parse_error_id = error_id_type::INCOMPLETE_PARSE;

    //using parse_context_extension_type = empty_parse_context_extension;

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
};


using lexer_type = lexer<source_type, calculator_lexer_grammar>;


static void test_tokenization() {
    {
        source_type source = "1@";
        auto result = lexer_type::parse(source);
    }
}


class calculator_parser_grammar {
public:
    using lexer_grammar = calculator_lexer_grammar;

    enum match_id_type {
        NUM,
        ADD,
        SUB,
        MUL,
        DIV
    };

    enum error_id_type {
        INVALID_TOKEN,
        INVALID_UNARY_EXPRESSION,
        INCOMPLETE_PARSE
    };

    //static constexpr error_id_type incomplete_parse_error_id = error_id_type::INCOMPLETE_PARSE;

    static error_id_type translate_lexer_error_id(lexer_type::error_id_type error) {
        switch (error) {
            case lexer_type::error_id_type::INVALID_TOKEN:
                return error_id_type::INVALID_TOKEN;

            default:
                break;
        }

        throw std::invalid_argument("invalid lexer error id");
    }

    template <class ParseContext>
    parse_result parse(ParseContext& pc) const noexcept {
        return instance<ParseContext>().parse(pc);
    }

private:
    template <class ParseContext>
    class instance {
    public:
        instance() {
            const auto val
                = lexer_grammar::match_id_type::LEFT_PARENTHESIS >> add >> lexer_grammar::match_id_type::RIGHT_PARENTHESIS
                | terminal(lexer_grammar::match_id_type::NUMBER)->*match_id_type::NUM;

            const auto val1
                = val
                | error(error_id_type::INVALID_UNARY_EXPRESSION) >> false;

            mul = (mul >> lexer_grammar::match_id_type::MUL >> val1)->*match_id_type::MUL
                | (mul >> lexer_grammar::match_id_type::DIV >> val1)->*match_id_type::DIV
                | val1;

            add = (add >> lexer_grammar::match_id_type::ADD >> mul)->*match_id_type::ADD
                | (add >> lexer_grammar::match_id_type::SUB >> mul)->*match_id_type::SUB
                | mul;
        }

        parse_result parse(ParseContext& pc) noexcept {
            return add.parse(pc);
        }

    private:
        rule<ParseContext> mul;
        rule<ParseContext> add;
    };

public:
    static double eval(const ast_node_ptr_type<match_id_type, lexer_type::iterator_type>& ast_node) {
        switch (ast_node->id()) {
            case match_id_type::NUM: {
                std::stringstream stream;
                stream << ast_node->source();
                double v;
                stream >> v;
                return v;
            }

            case match_id_type::ADD:
                assert(ast_node->children().size() == 2);
                return eval(ast_node->children()[0]) + eval(ast_node->children()[1]);

            case match_id_type::SUB:
                assert(ast_node->children().size() == 2);
                return eval(ast_node->children()[0]) - eval(ast_node->children()[1]);

            case match_id_type::MUL:
                assert(ast_node->children().size() == 2);
                return eval(ast_node->children()[0]) * eval(ast_node->children()[1]);

            case match_id_type::DIV:
                assert(ast_node->children().size() == 2);
                return eval(ast_node->children()[0]) / eval(ast_node->children()[1]);
        }

        throw std::invalid_argument("invalid ast node id");
    }
};


using parser_type = parser<source_type, calculator_lexer_grammar, calculator_parser_grammar>;


static void test_parsing() {
    {
        source_type source = "1";
        auto result = parser_type::parse(source);
        assert(result.success);
        assert(result.ast_nodes.size() == 1);
        assert(calculator_parser_grammar::eval(result.ast_nodes[0]) == 1.0);
        assert(result.errors.size() == 0);
    }

    {
        source_type source = "1+2";
        auto result = parser_type::parse(source);
        assert(result.success);
        assert(result.ast_nodes.size() == 1);
        assert(calculator_parser_grammar::eval(result.ast_nodes[0]) == 1.0 + 2.0);
        assert(result.errors.size() == 0);
    }

    {
        source_type source = "1+2*3";
        auto result = parser_type::parse(source);
        assert(result.success);
        assert(result.ast_nodes.size() == 1);
        assert(calculator_parser_grammar::eval(result.ast_nodes[0]) == 1.0 + 2.0 * 3.0);
        assert(result.errors.size() == 0);
    }

    {
        source_type source = "1+2/3";
        auto result = parser_type::parse(source);
        assert(result.success);
        assert(result.ast_nodes.size() == 1);
        assert(calculator_parser_grammar::eval(result.ast_nodes[0]) == 1.0 + 2.0 / 3.0);
        assert(result.errors.size() == 0);
    }

    {
        source_type source = "1-2*3";
        auto result = parser_type::parse(source);
        assert(result.success);
        assert(result.ast_nodes.size() == 1);
        assert(calculator_parser_grammar::eval(result.ast_nodes[0]) == 1.0 - 2.0 * 3.0);
        assert(result.errors.size() == 0);
    }

    {
        source_type source = "1-2/3";
        auto result = parser_type::parse(source);
        assert(result.success);
        assert(result.ast_nodes.size() == 1);
        assert(calculator_parser_grammar::eval(result.ast_nodes[0]) == 1.0 - 2.0 / 3.0);
        assert(result.errors.size() == 0);
    }

    {
        source_type source = "1*2+3";
        auto result = parser_type::parse(source);
        assert(result.success);
        assert(result.ast_nodes.size() == 1);
        assert(calculator_parser_grammar::eval(result.ast_nodes[0]) == 1.0 * 2.0 + 3.0);
        assert(result.errors.size() == 0);
    }

    {
        source_type source = "1*2-3";
        auto result = parser_type::parse(source);
        assert(result.success);
        assert(result.ast_nodes.size() == 1);
        assert(calculator_parser_grammar::eval(result.ast_nodes[0]) == 1.0 * 2.0 - 3.0);
        assert(result.errors.size() == 0);
    }

    {
        source_type source = "1/2+3";
        auto result = parser_type::parse(source);
        assert(result.success);
        assert(result.ast_nodes.size() == 1);
        assert(calculator_parser_grammar::eval(result.ast_nodes[0]) == 1.0 / 2.0 + 3.0);
        assert(result.errors.size() == 0);
    }

    {
        source_type source = "1/2-3";
        auto result = parser_type::parse(source);
        assert(result.success);
        assert(result.ast_nodes.size() == 1);
        assert(calculator_parser_grammar::eval(result.ast_nodes[0]) == 1.0 / 2.0 - 3.0);
        assert(result.errors.size() == 0);
    }

    {
        source_type source = "@+2";
        auto result = parser_type::parse(source);
        assert(!result.success);
        assert(result.ast_nodes.size() == 0);
        assert(result.errors.size() == 2);
        assert(result.errors[0].id() == calculator_parser_grammar::error_id_type::INVALID_TOKEN);
        assert(result.errors[1].id() == calculator_parser_grammar::error_id_type::INVALID_UNARY_EXPRESSION);
    }

}


void test_lexer_parser() {
    test_tokenization();
    test_parsing();
}
