#ifndef PARSERLIB_EBNF_GRAMMAR_HPP
#define PARSERLIB_EBNF_GRAMMAR_HPP


#include "case_sensitive_comparator.hpp"
#include "common_lexer_expressions.hpp"


namespace parserlib {


    class ebnf_grammar {
    public:
        class lexer {
        public:
            enum class TOKEN_ID {
                LEXER_IDENTIFIER,
                PARSER_IDENTIFIER,
                STRING,
                CHAR,
                RANGE,
                ASSIGNMENT,
                GROUP_START,
                GROUP_END,
                REPETITION_START,
                REPETITION_END,
                OPTIONAL_START,
                OPTIONAL_END,
                LOOP_0,
                LOOP_1,
                OPTIONAL,
                BRANCH,
                EXCLUSION,
                TERMINATOR
            };

            using token_id_type = TOKEN_ID;

            using character_comparator_type = case_sensitive_comparator;

            enum class ERROR_ID {
                SYNTAX_ERROR
            };

            using error_id_type = ERROR_ID;

            template <class Context>
            class grammar {
            public:
                grammar() {
                }

                bool parse(Context& context) const {
                    return get_grammar().parse(context);
                }

            private:
                auto get_grammar() const {
                    //whitespace
                    auto newline = parserlib::newline('\n');
                    auto block_comment = parserlib::block_comment("(*", newline | any(), "*)");
                    auto whitespace = block_comment | newline | parserlib::whitespace();

                    //identifiers
                    auto identifier = [](const auto& letter) { return (letter | '_') >> *(letter | parserlib::digit() | '_' | '-'); };
                    auto lexer_identifier = identifier(parserlib::uppercase_letter())->*TOKEN_ID::LEXER_IDENTIFIER;
                    auto parser_identifier = identifier(parserlib::lowercase_letter())->*TOKEN_ID::PARSER_IDENTIFIER;

                    //string
                    auto escaped_char = terminal("\\\"") | "\\\'" | "\\n" | "\\r" | "\\t" | "\\v" | "\\f" | "\\0";
                    auto hex_digit = one_of("0123456789abcdefABCDEF");
                    auto hex_char = (terminal("\\x") | "\\X") >> hex_digit >> -(hex_digit >> -(hex_digit >> -hex_digit));
                    auto string_char = escaped_char | hex_char | any();
                    auto string = ('\"' >> *((string_char | skip_to(string_char, ERROR_ID::SYNTAX_ERROR)) - '"') >> '\"')->*TOKEN_ID::STRING;

                    //char
                    auto character = (terminal('\'') >> ((string_char - '\'') | skip_to('\'', ERROR_ID::SYNTAX_ERROR)) >> '\'')->*TOKEN_ID::CHAR;

                    //range
                    auto range = terminal("..")->*TOKEN_ID::RANGE;

                    //single character terminals
                    auto assignment = terminal('=')->*TOKEN_ID::ASSIGNMENT;
                    auto groupStart = terminal('(')->*TOKEN_ID::GROUP_START;
                    auto groupEnd = terminal(')')->*TOKEN_ID::GROUP_END;
                    auto repetitionStart = terminal('{')->*TOKEN_ID::REPETITION_START;
                    auto repetitionEnd = terminal('}')->*TOKEN_ID::REPETITION_END;
                    auto optionalStart = terminal('[')->*TOKEN_ID::OPTIONAL_START;
                    auto optionalEnd = terminal(']')->*TOKEN_ID::OPTIONAL_END;
                    auto loop0 = terminal('*')->*TOKEN_ID::LOOP_0;
                    auto loop1 = terminal('+')->*TOKEN_ID::LOOP_1;
                    auto optional = terminal('?')->*TOKEN_ID::OPTIONAL;
                    auto branch = terminal('|')->*TOKEN_ID::BRANCH;
                    auto exclusion = terminal('-')->*TOKEN_ID::EXCLUSION;
                    auto terminator = terminal(';')->*TOKEN_ID::TERMINATOR;

                    //tokens
                    auto token = lexer_identifier
                        | parser_identifier
                        | string
                        | character
                        | range
                        | assignment
                        | groupStart
                        | groupEnd
                        | repetitionStart
                        | repetitionEnd
                        | optionalStart
                        | optionalEnd
                        | loop0
                        | loop1
                        | optional
                        | branch
                        | exclusion
                        | terminator;

                    //grammar
                    return *(whitespace | token | skip_to(whitespace | token, ERROR_ID::SYNTAX_ERROR)) >> end();
                }
            };
        };

        class parser {
        public:
            enum class AST_ID {
                STRING,
                LEXER_IDENTIFIER,
                LEXER_DECLARATION,
                PARSER_IDENTIFIER,
                PARSER_DECLARATION,
                OPTIONAL,
                LOOP_0,
                LOOP_1,
                GROUP,
                BRANCH,
                SEQUENCE,
                EXCLUSION,
                CHAR,
                CHAR_RANGE
            };

            using ast_id_type = AST_ID;

            enum class ERROR_ID {
                SYNTAX_ERROR
            };

            using error_id_type = ERROR_ID;

            template <class Context>
            class grammar {
            public:
                bool parse(Context& context) {
                    auto grammar = get_grammar();
                    return grammar.parse(context);
                }

            private:
                rule<Context> m_lexer_branch_expression;
                rule<Context> m_parser_branch_expression;

                auto get_grammar() {
                    //lexer and parser declarations differ; lexer declarations require ALL_CAPS for lexer tokens.
                    auto create_declaration = [&](const auto& identifier, rule<Context>& branch_expression, const auto& extra_values) {
                        const auto string = terminal(lexer::TOKEN_ID::STRING)->*AST_ID::STRING;

                        const auto group = (lexer::TOKEN_ID::GROUP_START >> branch_expression >> lexer::TOKEN_ID::GROUP_END)->*AST_ID::GROUP;

                        const auto optional_group = (lexer::TOKEN_ID::OPTIONAL_START >> branch_expression >> lexer::TOKEN_ID::OPTIONAL_END)->*AST_ID::OPTIONAL;

                        const auto repetition_group = (lexer::TOKEN_ID::REPETITION_START >> branch_expression >> lexer::TOKEN_ID::REPETITION_END)->*AST_ID::LOOP_0;

                        const auto character = terminal(lexer::TOKEN_ID::CHAR)->*AST_ID::CHAR;

                        const auto char_range = (character >> lexer::TOKEN_ID::RANGE >> character)->*AST_ID::CHAR_RANGE;

                        const auto value
                            = identifier
                            | string
                            | char_range
                            | character
                            | group
                            | optional_group
                            | repetition_group
                            | extra_values;

                        const auto optional = (value >> lexer::TOKEN_ID::OPTIONAL)->*AST_ID::OPTIONAL;

                        const auto loop_0 = (value >> lexer::TOKEN_ID::LOOP_0)->*AST_ID::LOOP_0;

                        const auto loop_1 = (value >> lexer::TOKEN_ID::LOOP_1)->*AST_ID::LOOP_1;

                        const auto unary_expression
                            = optional
                            | loop_0
                            | loop_1
                            | value;

                        const auto exclusion = (unary_expression >> lexer::TOKEN_ID::EXCLUSION >> unary_expression)->*AST_ID::EXCLUSION;

                        const auto binary_expression
                            = exclusion
                            | unary_expression;

                        const auto sequence = (binary_expression >> +binary_expression)->*AST_ID::SEQUENCE;

                        const auto sequence_expression
                            = sequence
                            | binary_expression;

                        branch_expression
                            = (branch_expression >> lexer::TOKEN_ID::BRANCH >> sequence_expression)->*AST_ID::BRANCH
                            | sequence_expression;

                        return identifier >> terminal(lexer::TOKEN_ID::ASSIGNMENT) >> branch_expression >> lexer::TOKEN_ID::TERMINATOR;
                    };

                    const auto lexer_identifier = terminal(lexer::TOKEN_ID::LEXER_IDENTIFIER)->*AST_ID::LEXER_IDENTIFIER;
                    const auto parser_identifier = terminal(lexer::TOKEN_ID::PARSER_IDENTIFIER)->*AST_ID::PARSER_IDENTIFIER;

                    const auto lexer_declaration = create_declaration(lexer_identifier, m_lexer_branch_expression, false)->*AST_ID::LEXER_DECLARATION;
                    const auto parser_declaration = create_declaration(parser_identifier, m_parser_branch_expression, lexer_identifier)->*AST_ID::PARSER_DECLARATION;

                    return (*(lexer_declaration | parser_declaration | skip_after(lexer::TOKEN_ID::TERMINATOR, ERROR_ID::SYNTAX_ERROR))) >> end();
                }
            }; //class grammar
        
        }; //class parser

    }; //class ebnf_grammar


} //namespace parserlib


#endif //PARSERLIB_EBNF_GRAMMAR_HPP
