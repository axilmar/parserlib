#ifndef PARSERLIB_EBNF_HPP
#define PARSERLIB_EBNF_HPP


#include "parserlib.hpp"


namespace parserlib {


    /**
     * An EBNF (extended Backus - Naur) parser.
     * 
     * Examples:
     * 
     *    (* a parser declaration *)
     *      <class-declaration> ::= 'class' %identifier% '{' <class-members> '}';
     * 
     *      (* alternative declaration without the angle brackets *)
     *      class-declaration ::= 'class' %identifier% '{' class-members '}';
     * 
     *      (* alternative declaration without '::=' *)
     *      class-declaration = 'class' %identifier% '{' class-members '}';
     * 
     *      (* alternative declaration without ';' *)
     *      class-declaration = 'class' %identifier% '{' class-members '}'

     *      (* alternative declaration with double quotes *)
     *      class-declaration = "class" %identifier% "{" class-members "}"
     * 
     *      (* a lexer declaration *)
     *      %identifier% = %letter% (%letter% | %digit% | '_')*
     * 
     * The ebnf parser supports:
     *      - parser and lexer rules (lexer rule names are enclosed in '%').
     *      - assignment via '::=' or '=' or ':'.
     *      - optional terminator ';' or '.'; if ommitted, then a blank line should follow.
     *      - sequences with optional separator ','.
     *      - character ranges: e.g. 'a' .. 'z'.
     *      - multipliers: e.g. 3 * some_rule.
     *      - range multipliers: e.g. 3..5 * some_rule.
     *      - identifiers can have '-' in them, as well as '_'.
     *      - strings enclosed in single or double quotes.
     *      - comments that start with '(*' and end with '*)'; they can be multiline.
     */
    class ebnf {
    public:
        /** Token Id. */
        enum class token_id {
            NEWLINE,
            LEXER_IDENTIFIER,
            PARSER_IDENTIFIER,
            NUMBER,
            CHAR,
            STRING,
            RANGE,
            ASSIGN,
            PIPE,
            LEFT_PARENTHESIS,
            RIGHT_PARENTHESIS,
            LEFT_CURLY_BRACKET,
            RIGHT_CURLY_BRACKET,
            LEFT_SQUARE_BRACKET,
            RIGHT_SQUARE_BRACKET,
            QUESTION_MARK,
            ASTERISK,
            PLUS,
            MINUS,
            TERMINATOR,
            COMMA,
            AMBERSAND,
            EXCLAMATION_MARK
        };

        /** AST id. */
        enum class ast_id {
            OPTIONAL,
            LOOP_0_OR_MORE_TIMES,
            LOOP_1_OR_MORE_TIMES,
            LEXER_NAME,
            PARSER_NAME,
            CHAR_RANGE_MIN,
            CHAR_RANGE_MAX,
            CHAR_RANGE,
            TERMINAL,
            LOGICAL_AND,
            LOGICAL_NOT,
            EXCLUSION,
            LOOP,
            MULTIPLE,
            SEQUENCE,
            CHOICE,
            LEXER_RULE,
            PARSER_RULE,
            COUNT,
            MIN_COUNT,
            MAX_COUNT
        };

        /** Error Id. */
        enum class error_id {
            INVALID_CHARACTERS
        };

        /**
         * The lexer.
         * @param Source the type of source.
         */
        template <class Source>
        class lexer {
        public:
            /** The parse context. */
            using parse_context = parserlib::parse_context<Source, token_id, error_id, text_position>;

            /** The rule type. */
            using rule_type = rule<parse_context>;

            /** The lexer grammar. */
            class grammar : public rule_type {
            public:
                using rule_type::operator =;

                grammar() {
                    const auto newline_term = parserlib::newline('\n');
                    const auto newline = newline_term->*token_id::NEWLINE;

                    const auto whitespace = range('\0', ' ');

                    const auto comment = "(*" >> *((newline_term | any()) - "*)") >> "*)";

                    const auto letter = range('a', 'z') | range('A', 'Z');
                    const auto digit = range('0', '9');
                    const auto identifier = letter >> *(letter | digit | '-' | '_');

                    const auto lexer_identifier = ('%' >> identifier >> '%')->*token_id::LEXER_IDENTIFIER;
                    
                    const auto parser_identifier 
                        = ('<' >> identifier >> '>')->*token_id::PARSER_IDENTIFIER
                        | identifier->*token_id::PARSER_IDENTIFIER
                        ;

                    const auto number
                        = (+digit)->*token_id::NUMBER
                        ;

                    const auto escape_character
                        = terminal("\\\\")
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

                    const auto valid_string_character
                        = escape_character
                        | hex_character
                        | any() - '\\'
                        ;

                    const auto string_character1
                        = valid_string_character
                        | error(error_id::INVALID_CHARACTERS, skip_before(valid_string_character | '\''))
                        ;

                    const auto character
                        = ('\'' >> (string_character1 - '\'') >> '\'')->*token_id::CHAR
                        ;

                    const auto string_character2
                        = valid_string_character
                        | error(error_id::INVALID_CHARACTERS, skip_before(valid_string_character | '\"'))
                        ;

                    const auto string
                        = ('\'' >> *(string_character1 - '\'') >> '\'')->*token_id::STRING
                        | ('\"' >> *(string_character2 - '\"') >> '\"')->*token_id::STRING
                        ;

                    const auto token
                        = newline
                        | whitespace
                        | comment
                        | lexer_identifier
                        | parser_identifier
                        | number
                        | character
                        | string
                        | terminal("::=")->*token_id::ASSIGN
                        | terminal("..")->*token_id::RANGE
                        | terminal('=')->*token_id::ASSIGN
                        | terminal(':')->*token_id::ASSIGN
                        | terminal('|')->*token_id::PIPE
                        | terminal('(')->*token_id::LEFT_PARENTHESIS
                        | terminal(')')->*token_id::RIGHT_PARENTHESIS
                        | terminal('{')->*token_id::LEFT_CURLY_BRACKET
                        | terminal('}')->*token_id::RIGHT_CURLY_BRACKET
                        | terminal('[')->*token_id::LEFT_SQUARE_BRACKET
                        | terminal(']')->*token_id::RIGHT_SQUARE_BRACKET
                        | terminal('?')->*token_id::QUESTION_MARK
                        | terminal('*')->*token_id::ASTERISK
                        | terminal('+')->*token_id::PLUS
                        | terminal('-')->*token_id::MINUS
                        | terminal(';')->*token_id::TERMINATOR
                        | terminal('.')->*token_id::TERMINATOR
                        | terminal(',')->*token_id::COMMA
                        | terminal('&')->*token_id::AMBERSAND
                        | terminal('!')->*token_id::EXCLAMATION_MARK
                        ;

                    *this = *(token | error(error_id::INVALID_CHARACTERS, skip_before(token))) >> end();

                    this->set_name("lexer");
                }
            };
        };

        /**
         * The parser.
         * @param Source the type of source.
         */
        template <class Source>
        class parser {
        public:
            using lexer = ebnf::lexer<Source>;
            using lexer_parse_context = typename lexer::parse_context;
            using lexer_match_container_type = typename lexer_parse_context::match_container_type;

            /** The parser parse context. */
            using parse_context = parserlib::parse_context<lexer_match_container_type, ast_id, error_id>;

            /** The parser rule type. */
            using rule_type = parserlib::rule<parse_context>;

            /** The parser grammar. */
            class grammar : public rule_type {
            private:
                rule_type m_choice, m_sequence;

            public:
                using rule_type::operator =;

                grammar() {
                    const auto newline = terminal(token_id::NEWLINE);

                    const auto choice_term = terminal(token_id::LEFT_PARENTHESIS) >> m_choice >> token_id::RIGHT_PARENTHESIS;
                    const auto optional_term = (terminal(token_id::LEFT_SQUARE_BRACKET) >> m_choice >> token_id::RIGHT_SQUARE_BRACKET)->*ast_id::OPTIONAL;
                    const auto repetition_term = (terminal(token_id::LEFT_CURLY_BRACKET) >> m_choice >> token_id::RIGHT_CURLY_BRACKET)->*ast_id::LOOP_0_OR_MORE_TIMES;

                    const auto lexer_name = terminal(token_id::LEXER_IDENTIFIER)->*ast_id::LEXER_NAME;
                    const auto parser_name = terminal(token_id::PARSER_IDENTIFIER)->*ast_id::PARSER_NAME;

                    const auto char_range = (terminal(token_id::CHAR)->*ast_id::CHAR_RANGE_MIN >> token_id::RANGE >> terminal(token_id::CHAR)->*ast_id::CHAR_RANGE_MAX)->*ast_id::CHAR_RANGE;
                    const auto char_ = terminal(token_id::CHAR)->*ast_id::TERMINAL;
                    const auto string = terminal(token_id::STRING)->*ast_id::TERMINAL;

                    const auto term
                        = choice_term
                        | optional_term
                        | repetition_term
                        | lexer_name
                        | parser_name
                        | char_range
                        | char_
                        | string
                        ;

                    const auto optional
                        = (token_id::QUESTION_MARK >> term)->*ast_id::OPTIONAL
                        | (term >> token_id::QUESTION_MARK)->*ast_id::OPTIONAL
                        ;

                    const auto loop_0_or_more
                        = (token_id::ASTERISK >> term)->*ast_id::LOOP_0_OR_MORE_TIMES
                        | (term >> token_id::ASTERISK)->*ast_id::LOOP_0_OR_MORE_TIMES
                        ;

                    const auto loop_1_or_more
                        = (token_id::PLUS >> term)->*ast_id::LOOP_1_OR_MORE_TIMES
                        | (term >> token_id::PLUS)->*ast_id::LOOP_1_OR_MORE_TIMES
                        ;

                    const auto logical_and
                        = (token_id::AMBERSAND >> term)->*ast_id::LOGICAL_AND
                        ;

                    const auto logical_not
                        = (token_id::EXCLAMATION_MARK >> term)->*ast_id::LOGICAL_NOT
                        ;

                    const auto exclusion
                        = (term >> token_id::MINUS >> term)->*ast_id::EXCLUSION
                        ;

                    const auto loop
                        = (terminal(token_id::NUMBER)->*ast_id::MIN_COUNT >> token_id::RANGE >> terminal(token_id::NUMBER)->*ast_id::MAX_COUNT >> token_id::ASTERISK >> term)->*ast_id::LOOP
                        ;

                    const auto multiple
                        = (terminal(token_id::NUMBER)->*ast_id::COUNT >> token_id::ASTERISK >> term)->*ast_id::MULTIPLE
                        ;

                    const auto factor
                        = optional
                        | loop_0_or_more
                        | loop_1_or_more
                        | logical_and 
                        | logical_not
                        | loop
                        | multiple
                        | exclusion
                        | term
                        ;

                    m_sequence
                        = (factor >> +(-terminal(token_id::COMMA) >> factor))->*ast_id::SEQUENCE
                        | factor
                        ;

                    m_choice
                        = (m_sequence >> +(*newline >> token_id::PIPE >> m_sequence))->*ast_id::CHOICE
                        | m_sequence
                        ;

                    const auto terminator
                        = newline >> newline
                        | token_id::TERMINATOR
                        | newline >> end()
                        | end()
                        ;

                    const auto lexer_rule
                        = (lexer_name >> token_id::ASSIGN >> m_choice >> terminator)->*ast_id::LEXER_RULE
                        ;

                    const auto parser_rule
                        = (parser_name >> token_id::ASSIGN >> m_choice >> terminator)->*ast_id::PARSER_RULE
                        ;

                    const auto declaration
                        = lexer_rule 
                        | parser_rule
                        | token_id::TERMINATOR
                        | token_id::NEWLINE
                        ;

                    *this = *declaration >> end();

                    m_sequence.set_name("sequence");
                    m_choice.set_name("choice");
                    this->set_name("parser");
                }
            };
        };

        /**
         * Parses a source that contains an EBNF-like grammar.
         * @param src the source.
         * @param ast_factory the ast factory to use.
         * @return a tokenize and parse result.
         */
        template <class Source, class ASTFactory>
        static auto parse(Source& src, const ASTFactory& ast_factory) {
            typename lexer<Source>::grammar lexer_grammar;
            typename parser<Source>::grammar parser_grammar;
            return parserlib::tokenize_and_parse(src, lexer_grammar, parser_grammar, ast_factory);
        }

        /**
         * Parses a source that contains an EBNF-like grammar.
         * @param src the source.
         * @return a tokenize and parse result.
         */
        template <class Source>
        static auto parse(Source& src) {
            typename lexer<Source>::grammar lexer_grammar;
            typename parser<Source>::grammar parser_grammar;
            return parserlib::tokenize_and_parse(src, lexer_grammar, parser_grammar);
        }
    };


    inline std::string get_id_name(ebnf::token_id id) {
        switch (id) {
            case ebnf::token_id::NEWLINE: return "NEWLINE";
            case ebnf::token_id::LEXER_IDENTIFIER: return "LEXER_IDENTIFIER";
            case ebnf::token_id::PARSER_IDENTIFIER: return "PARSER_IDENTIFIER";
            case ebnf::token_id::NUMBER: return "NUMBER";
            case ebnf::token_id::CHAR: return "CHAR";
            case ebnf::token_id::STRING: return "STRING";
            case ebnf::token_id::RANGE: return "RANGE";
            case ebnf::token_id::ASSIGN: return "ASSIGN";
            case ebnf::token_id::PIPE: return "PIPE";
            case ebnf::token_id::LEFT_PARENTHESIS: return "LEFT_PARENTHESIS";
            case ebnf::token_id::RIGHT_PARENTHESIS: return "RIGHT_PARENTHESIS";
            case ebnf::token_id::LEFT_CURLY_BRACKET: return "LEFT_CURLY_BRACKET";
            case ebnf::token_id::RIGHT_CURLY_BRACKET: return "RIGHT_CURLY_BRACKET";
            case ebnf::token_id::LEFT_SQUARE_BRACKET: return "LEFT_SQUARE_BRACKET";
            case ebnf::token_id::RIGHT_SQUARE_BRACKET: return "RIGHT_SQUARE_BRACKET";
            case ebnf::token_id::QUESTION_MARK: return "QUESTION_MARK";
            case ebnf::token_id::ASTERISK: return "ASTERISK";
            case ebnf::token_id::PLUS: return "PLUS";
            case ebnf::token_id::MINUS: return "MINUS";
            case ebnf::token_id::TERMINATOR: return "TERMINATOR";
            case ebnf::token_id::COMMA: return "COMMA";
            case ebnf::token_id::AMBERSAND: return "AMBERSAND";
            case ebnf::token_id::EXCLAMATION_MARK: return "EXCLAMATION_MARK";
        }
        throw std::logic_error("ebnf: get_id_name(ebnf::token_id): invalid id");
    }


    inline std::string get_id_name(ebnf::ast_id id) {
        switch (id) {
            case ebnf::ast_id::OPTIONAL: return "OPTIONAL";
            case ebnf::ast_id::LOOP_0_OR_MORE_TIMES: return "LOOP_0_OR_MORE_TIMES";
            case ebnf::ast_id::LOOP_1_OR_MORE_TIMES: return "LOOP_1_OR_MORE_TIMES";
            case ebnf::ast_id::LEXER_NAME: return "LEXER_NAME";
            case ebnf::ast_id::PARSER_NAME: return "PARSER_NAME";
            case ebnf::ast_id::CHAR_RANGE_MIN: return "CHAR_RANGE_MIN";
            case ebnf::ast_id::CHAR_RANGE_MAX: return "CHAR_RANGE_MAX";
            case ebnf::ast_id::CHAR_RANGE: return "CHAR_RANGE";
            case ebnf::ast_id::TERMINAL: return "TERMINAL";
            case ebnf::ast_id::LOGICAL_AND: return "LOGICAL_AND";
            case ebnf::ast_id::LOGICAL_NOT: return "LOGICAL_NOT";
            case ebnf::ast_id::EXCLUSION: return "EXCLUSION";
            case ebnf::ast_id::LOOP: return "LOOP";
            case ebnf::ast_id::MULTIPLE: return "MULTIPLE";
            case ebnf::ast_id::SEQUENCE: return "SEQUENCE";
            case ebnf::ast_id::CHOICE: return "CHOICE";
            case ebnf::ast_id::LEXER_RULE: return "LEXER_RULE";
            case ebnf::ast_id::PARSER_RULE: return "PARSER_RULE";
            case ebnf::ast_id::COUNT: return "COUNT";
            case ebnf::ast_id::MIN_COUNT: return "MIN_COUNT";
            case ebnf::ast_id::MAX_COUNT: return "MAX_COUNT";
        }
        throw std::logic_error("ebnf: get_id_name(ebnf::ast_id): invalid id");
    }


} //namespace parserlib


#endif //PARSERLIB_EBNF_HPP
