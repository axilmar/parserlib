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

            static auto grammar() {
                //whitespace
                auto newline = parserlib::newline('\n');
                auto block_comment = parserlib::block_comment("(*", newline | any(), "*)");
                auto whitespace = block_comment | newline | parserlib::whitespace();
                
                //identifiers
                auto identifier = [](const auto& letter) { return letter >> *(letter | parserlib::digit() | '_' | '-'); };
                auto lexer_identifier = identifier(parserlib::uppercase_letter())->*TOKEN_ID::LEXER_IDENTIFIER;
                auto parser_identifier = identifier(parserlib::lowercase_letter())->*TOKEN_ID::PARSER_IDENTIFIER;

                //string
                auto escaped_char = terminal("\\\"") | "\\\'" | "\\n" | "\\r" | "\\t" | "\\v" | "\\f" | "\\0";
                auto hex_digit = one_of("0123456789abcdefABCDEF");
                auto hex_char = (terminal("\\x") | "\\X") >> hex_digit >> -(hex_digit >> -(hex_digit >> -hex_digit));
                auto string_char = escaped_char | hex_char | any();
                auto string = ('\"' >> *((string_char | skip_to(string_char, ERROR_ID::SYNTAX_ERROR)) - '"') >> '\"')->*TOKEN_ID::STRING;

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


} //namespace parserlib


#endif //PARSERLIB_EBNF_GRAMMAR_HPP
