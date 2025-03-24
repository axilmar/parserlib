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
          
            static auto block_comment() -> decltype(parserlib::block_comment("(*", newline('\n') | any(), "*)")) {
                return parserlib::block_comment("(*", newline('\n') | any(), "*)");
            }

            static auto whitespace() -> decltype(block_comment() | newline('\n') | parserlib::whitespace()) {
                return block_comment() | newline('\n') | parserlib::whitespace();
            }

            template <class Letter>
            static auto identifier(const Letter& letter) -> decltype(letter >> *(letter | parserlib::digit() | '_' | '-')) {
                return letter >> *(letter | parserlib::digit() | '_' | '-');
            }

            static auto lexer_identifier() -> decltype(identifier(parserlib::uppercase_letter())->*TOKEN_ID::LEXER_IDENTIFIER) {
                return identifier(parserlib::uppercase_letter())->*TOKEN_ID::LEXER_IDENTIFIER;
            }

            static auto parser_identifier() -> decltype(identifier(parserlib::lowercase_letter())->*TOKEN_ID::PARSER_IDENTIFIER) {
                return identifier(parserlib::lowercase_letter())->*TOKEN_ID::PARSER_IDENTIFIER;
            }

            static auto escaped_char() -> decltype(terminal("\\\"") | "\\\'" | "\\n" | "\\r" | "\\t" | "\\v" | "\\f" | "\\0") {
                return terminal("\\\"") | "\\\'" | "\\n" | "\\r" | "\\t" | "\\v" | "\\f" | "\\0";
            }

            static auto hex_digit() -> decltype(one_of("0123456789abcdefABCDEF")) {
                return one_of("0123456789abcdefABCDEF");
            }

            static auto hex_char() -> decltype((terminal("\\x") | "\\X") >> hex_digit() >> -(hex_digit() >> -(hex_digit() >> -hex_digit()))) {
                return (terminal("\\x") | "\\X") >> hex_digit() >> -(hex_digit() >> -(hex_digit() >> -hex_digit()));
            }

            static auto string_char_no_error() -> decltype(escaped_char() | hex_char() | any()) {
                return escaped_char() | hex_char() | any();
            }

            template <class ErrorID>
            static auto string_char(ErrorID syntax_error_id) -> decltype(on_error_continue_after(string_char_no_error(), syntax_error_id, string_char_no_error())) {
                return on_error_continue_after(string_char_no_error(), syntax_error_id, string_char_no_error());
            }

            template <class ErrorID>
            static auto string(ErrorID syntax_error_id) -> decltype(('\"' >> *(string_char(syntax_error_id) - '\"') >> '\"')->*TOKEN_ID::STRING) {
                return ('\"' >> *(string_char(syntax_error_id) - '\"') >> '\"')->*TOKEN_ID::STRING;
            }

            static auto assignment() -> decltype(terminal('=')->*TOKEN_ID::ASSIGNMENT) {
                return terminal('=')->*TOKEN_ID::ASSIGNMENT;
            }

            static auto groupStart() -> decltype(terminal('(')->*TOKEN_ID::GROUP_START) {
                return terminal('(')->*TOKEN_ID::GROUP_START;
            }

            static auto groupEnd() -> decltype(terminal(')')->*TOKEN_ID::GROUP_END) {
                return terminal(')')->*TOKEN_ID::GROUP_END;
            }

            static auto repetitionStart() -> decltype(terminal('{')->*TOKEN_ID::REPETITION_START) {
                return terminal('{')->*TOKEN_ID::REPETITION_START;
            }

            static auto repetitionEnd() -> decltype(terminal('}')->*TOKEN_ID::REPETITION_END) {
                return terminal('}')->*TOKEN_ID::REPETITION_END;
            }

            static auto optionalStart() -> decltype(terminal('[')->*TOKEN_ID::OPTIONAL_START) {
                return terminal('[')->*TOKEN_ID::OPTIONAL_START;
            }

            static auto optionalEnd() -> decltype(terminal(']')->*TOKEN_ID::OPTIONAL_END) {
                return terminal(']')->*TOKEN_ID::OPTIONAL_END;
            }

            static auto loop0() -> decltype(terminal('*')->*TOKEN_ID::LOOP_0) {
                return terminal('*')->*TOKEN_ID::LOOP_0;
            }

            static auto loop1() -> decltype(terminal('+')->*TOKEN_ID::LOOP_1) {
                return terminal('+')->*TOKEN_ID::LOOP_1;
            }

            static auto optional() -> decltype(terminal('?')->*TOKEN_ID::OPTIONAL) {
                return terminal('?')->*TOKEN_ID::OPTIONAL;
            }

            static auto branch() -> decltype(terminal('|')->*TOKEN_ID::BRANCH) {
                return terminal('|')->*TOKEN_ID::BRANCH;
            }

            static auto exclusion() -> decltype(terminal('-')->*TOKEN_ID::EXCLUSION) {
                return terminal('-')->*TOKEN_ID::EXCLUSION;
            }

            static auto terminator() -> decltype(terminal(';')->*TOKEN_ID::TERMINATOR) {
                return terminal(';')->*TOKEN_ID::TERMINATOR;
            }

            template <class ErrorID>
            static auto token(ErrorID syntax_error_id) {
                return lexer_identifier()
                     | parser_identifier()
                     | string(syntax_error_id)
                     | assignment()
                     | groupStart()
                     | groupEnd()
                     | repetitionStart()
                     | repetitionEnd()
                     | optionalStart()
                     | optionalEnd()
                     | loop0()
                     | loop1()
                     | optional()
                     | branch()
                     | exclusion()
                     | terminator();
            }

            template <class ErrorID>
            static auto grammar(ErrorID syntax_error_id) {
                const auto valid_token_start_character = parserlib::letter() | one_of("\n\"\'=(){}[]*+?|-;");
                return *(whitespace() | on_error_continue_after(token(syntax_error_id), syntax_error_id, valid_token_start_character)) >> end();
            }
        };
    };


} //namespace parserlib


#endif //PARSERLIB_EBNF_GRAMMAR_HPP
