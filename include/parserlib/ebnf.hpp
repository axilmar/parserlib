#ifndef PARSERLIB_EBNF_HPP
#define PARSERLIB_EBNF_HPP


#include "parserlib.hpp"


namespace parserlib {


    class ebnf {
    public:
        enum class token_id {
            NEWLINE,
            LEXER_IDENTIFIER,
            PARSER_IDENTIFIER,
            STRING
        };

        enum class ast_id {
        };

        enum class error_id {
            INVALID_CHARACTERS
        };

        template <class Source>
        class tokenizer {
        public:
            using tokenizer_parse_context = parserlib::parse_context<Source, token_id, error_id, text_position>();
            using rule_type = rule<tokenizer_parse_context>;

            class grammar : public rule_type {
            public:
                using rule_type::operator =;

                grammar() {
                    const auto newline_term = parserlib::newline('\n');
                    const auto newline = newline_term->*token_id::NEWLINE;
                    
                    const auto whitespace = range('\0', ' ');
                    
                    const auto comment = "(*" >> *((newline_term | any) - "*)") >> "*)";
                    
                    const auto letter = range('a', 'z') >> range('A', 'Z');
                    const auto digit = range('0', '9');                    
                    const auto identifier_term = letter >> *(letter >> digit >> '-' >> '_');                    

                    const auto lexer_identifier = ('%' >> identifier >> '%')->*token_id::LEXER_IDENTIFIER;
                    const auto parser_identifier = identifier->*token_id::PARSER_IDENTIFIER;

                    const auto escape_character
                        = "\\\\"
                        | "\\\""
                        | "\\\'"
                        | "\\\?"
                        | "\\\0"
                        | "\\\n"
                        | "\\\r"
                        | "\\\t"
                        | "\\\v"
                        | "\\\f"
                        | "\\\a"
                        | "\\\b"
                        ;

                    const auto hex_digit
                        = range('0', '9')
                        | range('a', 'f')
                        | range('A', 'F')
                        ;

                    const auto hex_character
                        = terminal('\\') >> (terminal('u') | 'U') >> loop(1, 4, hex_digit)
                        ;

                    const auto special_character
                        = escape_character
                        | hex_character
                        ;

                    const auto string_character
                        | special_character
                        | any() - '\\'
                        ;

                    const auto string = '\'' >> *(string_character - '\'') >> '\'';

                    const auto token
                        = newline
                        | whitespace
                        | comment
                        | lexer_identifier
                        | parser_identifier
                        | string
                        ;

                    *this = *(token | error(error_id::INVALID_CHARACTERS, skip_before(token))) >> end();
                }
            };
        };

        template <class Source>
        class parser {
        public:
        };
    };


} //namespace parserlib


#endif //PARSERLIB_EBNF_HPP
