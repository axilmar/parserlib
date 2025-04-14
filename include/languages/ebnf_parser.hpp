#ifndef PARSERLIB_LANGUAGES_EBNF_PARSER_HPP
#define PARSERLIB_LANGUAGES_EBNF_PARSER_HPP


#include "parserlib.hpp"


namespace parserlib {


    using namespace parserlib;


    /**
     * An EBNF parser.
     *
     * It implements the following grammar (expressed in EBNF):
     *
     *      ebnf = rule*;
     *
     *      rule = parser_identifier, rule_definition_operator, expression, terminator;
     *
     *      rule_definition_operator = '::=' | '=' | ':';
     *
     *      expression = alternation;
     *
     *      terminator = ';' | '.' | '\n\n';
     *
     *      alternation = concatenation, {'|', concatenation};
     *
     *      concatenation = factor, {','?, factor};
     *
     *      factor = term, '?'
     *             | term, '*'
     *             | term, '+'
     *             | term, '-', term
     *             | integer, '*'?, term
     *             | single_character_string, '..', single_character_string
     *             | term;
     *
     *      integer = digit+ - '0';
     *
     *      term = '[', expression, ']'
     *           | '{', expression, '}'
     *           | '(', expression, ')'
     *           | lexer_identifier
     *           | parser_identifier
     *           | string_set
     *           | single_character_string
     *           | string;
     *
     *      lexer_identifier = '#', '<', identifier, '>'
     *                       | '#', identifier;
     *
     *      parser_identifier = '<', identifier, '>'
     *                        | identifier;
     * 
     *      identifier = letter, {letter | digit | '_' | '-'};
     *
     *      string_set = '%', {character - '%'}, '%'
     *
     *      single_character_string = "'", character - "'", "'";
     *
     *      string = '"', {character - '"'}, '"'
     *             | "'", {character - "'"}, "'";
     *
     *      character = escaped_character | letter | digit | symbol;
     *
     *      escaped_character = '\\' | '\"' | '\'' | '\n' | '\r' | '\t' | '\v' | '\f' | '\b';
     *
     *      symbol = one of "`~!@#$%^&*()-_=+[{]}|:;<,>.?/";
     *
     * Comments start with '(*' and end with '*)'.
     *
     * Letter is any character within the following sets:
     * 
     *      abcdefghijklmnopqrstuvwxyz
     *      ABCDEFGHIJKLMNOPQRSTUVWXYZ
     *
     * Digit is any character within the following set:
     * 
     *      0123456789
     *
     * The grammar is flexible enough to understand various versions of EBNF:
     *
     *  - identifiers can contain hyphens.
     *  - terminators can be a list of either a semicolon, or a dot, or two newline characters.
     *  - the concatenation operator is optional.
     *  - the rule definition operator can be one of '=', ':', '::='.
     *  - identifiers can optionally be enclosed in '<', '>'.
     *  - if an parser_identifier does not represent a rule, then it is considered to be a terminal.
     *  - Multiplying a non-zero integer with a term repeats the term the specified number of times.
     *
     * Furthemore, it allows for the following extensions:
     *
     *  - lexer rules, using the prefix operator '#'.
     *    Lexer rules can later be used to create a lexer for the specific grammar.
     *    Example:
     *
     *      #parser_identifier = letter {letter | digit | '_'};
     *
     *  - range, using the infix operator '..'. Example:
     *
     *      'a' .. 'z'
     *      '0' .. '9'
     *
     *  - set, using the symbol '%'. Example:
     *
     *      %0123456789%
     *      %0123456789abcdefABCDEF%
     */
    class ebnf_parser {
    public:
        /** Error id type. */
        enum class error_id_type {
            /** Invalid characters. */
            INVALID_CHARACTERS
        };

        /** The lexer grammar. */
        class lexer_grammar {
        public:
            /** The match id type. */
            enum class match_id_type {
                NEWLINE_TERMINATOR,
                LEXER_IDENTIFIER,
                PARSER_IDENTIFIER,
                STRING_SET,
                SINGLE_CHARACTER_STRING,
                STRING,
                INTEGER,
                RANGE_OPERATOR,
                GROUP_START,
                GROUP_END,
                OPTIONAL_START,
                OPTIONAL_END,
                REPETITION_START,
                REPETITION_END,
                OPTIONAL_OPERATOR,
                LOOP_0_OPERATOR,
                LOOP_1_OPERATOR,
                CONCATENATION_OPERATOR,
                ALTERNATION_OPERATOR,
                EXCLUSION_OPERATOR,
                DEFINITION_OPERATOR,
                TERMINATOR
            };

            /** The error id type. */
            using error_id_type = parserlib::ebnf_parser::error_id_type;

            /**
             * Parses the given input and creates the matches for the tokens.
             * @param pc the parse context.
             * @return the parsed result.
             */
            template <class ParseContext>
            parse_result parse(ParseContext& pc) const noexcept {
                const auto newline_terminator = terminal("\n\n")->*match_id_type::NEWLINE_TERMINATOR;

                const auto whitespace = +range('\0', ' ');

                const auto comment = terminal("(*") >> *(any() - "*)") >> "*)";

                const auto digit = range('0', '9');
                const auto lowercase_letter = range('a', 'z');
                const auto uppercase_letter = range('A', 'Z');
                const auto letter = lowercase_letter | uppercase_letter;
                const auto alnum = letter | digit;

                const auto identifier_grammar = (letter >> *(alnum | '_' | '-'));

                const auto lexer_identifier 
                    = terminal('#') >> '<' >> identifier_grammar->*match_id_type::LEXER_IDENTIFIER >> '>'
                    | terminal('#') >> identifier_grammar->*match_id_type::LEXER_IDENTIFIER;

                const auto parser_identifier 
                    = '<' >> identifier_grammar->*match_id_type::PARSER_IDENTIFIER >> '>'
                    | identifier_grammar->*match_id_type::PARSER_IDENTIFIER;

                const auto escaped_character_value = terminal("\\\\") | "\\\"" | "\\\'" | "\\n" | "\\r" | "\\t" | "\\v" | "\\f" | "\\b";

                const auto symbol = set("`~!@#$%^&*()-_=+[{]}|:;<,>.?/");

                const auto character_value = escaped_character_value | alnum | symbol;

                const auto string_set = '%' >> (*((character_value | "\\%") - '%'))->*match_id_type::STRING_SET >> '%';

                const auto single_character_string = ('\'' >> ((character_value - '\'')->*match_id_type::SINGLE_CHARACTER_STRING) >> '\'');

                const auto string 
                    = '\"' >> ((*(character_value - '\"'))->*match_id_type::STRING) >> '\"'
                    | '\'' >> ((*(character_value - '\''))->*match_id_type::STRING) >> '\'';
                
                const auto integer = (+digit - '0')->*match_id_type::INTEGER;

                const auto range_operator = terminal("..")->*match_id_type::RANGE_OPERATOR;

                const auto group_start = terminal('(')->*match_id_type::GROUP_START;
                const auto group_end = terminal(')')->*match_id_type::GROUP_END;

                const auto optional_start = terminal('[')->*match_id_type::OPTIONAL_START;
                const auto optional_end = terminal(']')->*match_id_type::OPTIONAL_END;

                const auto repetition_start = terminal('{')->*match_id_type::REPETITION_START;
                const auto repetition_end = terminal('}')->*match_id_type::REPETITION_END;

                const auto optional_operator = terminal('?')->*match_id_type::OPTIONAL_OPERATOR;

                const auto loop_0_operator = terminal('*')->*match_id_type::LOOP_0_OPERATOR;

                const auto loop_1_operator = terminal('+')->*match_id_type::LOOP_1_OPERATOR;

                const auto concatenation_operator = terminal(',')->*match_id_type::CONCATENATION_OPERATOR;

                const auto alternation_operator = terminal('|')->*match_id_type::ALTERNATION_OPERATOR;

                const auto exception_operator = terminal('-')->*match_id_type::EXCLUSION_OPERATOR;

                const auto definition_operator = (terminal("::=") | '=' | ':')->*match_id_type::DEFINITION_OPERATOR;

                const auto terminator = set(";.")->*match_id_type::TERMINATOR;

                const auto token
                    = newline_terminator
                    | whitespace
                    | comment
                    | lexer_identifier
                    | parser_identifier
                    | string_set
                    | single_character_string
                    | string
                    | integer
                    | range_operator
                    | group_start
                    | group_end
                    | optional_start
                    | optional_end
                    | repetition_start
                    | repetition_end
                    | optional_operator
                    | loop_0_operator
                    | loop_1_operator
                    | concatenation_operator
                    | alternation_operator
                    | exception_operator
                    | definition_operator
                    | terminator
                    | error(error_id_type::INVALID_CHARACTERS, skip_until(set("\n(<%\'\".[{?*+,|-:=#;.") | alnum | whitespace));

                const auto grammar = *token;

                return grammar.parse(pc);
            }
        };

        /** The parser grammar. */
        class parser_grammar {
        public:
            /** The match id type. */
            enum class match_id_type {
                LEXER_RULE,
                PARSER_RULE,
                ALTERNATION,
                CONCATENATION,
                OPTIONAL,
                LOOP_0,
                LOOP_1,
                EXCLUSION,
                MULTIPLICATION,
                GROUP,
                RANGE,
                LEXER_IDENTIFIER,
                PARSER_IDENTIFIER,
                STRING_SET,
                STRING,
                RANGE_CHARACTER,
                INTEGER
            };

            /**
             * Parses the given input and creates the matches for the AST.
             * @param pc the parse context.
             * @return the parse result.
             */
            template <class ParseContext>
            parse_result parse(ParseContext& pc) const noexcept {
                grammar<ParseContext> grammar_instance;
                return grammar_instance.parse(pc);
            }

        private:
            template <class ParseContext>
            class grammar {
            public:
                using match_id_type = parser_grammar::match_id_type;

                grammar() {
                    const auto optional_group = (lexer_grammar::match_id_type::OPTIONAL_START >> m_expression >> lexer_grammar::match_id_type::OPTIONAL_END)->*match_id_type::OPTIONAL;

                    const auto repetition_group = (lexer_grammar::match_id_type::REPETITION_START >> m_expression >> lexer_grammar::match_id_type::REPETITION_END)->*match_id_type::LOOP_0;

                    const auto group = (lexer_grammar::match_id_type::GROUP_START >> m_expression >> lexer_grammar::match_id_type::GROUP_END)->*match_id_type::GROUP;

                    const auto lexer_identifier = terminal(lexer_grammar::match_id_type::LEXER_IDENTIFIER)->*match_id_type::LEXER_IDENTIFIER;

                    const auto parser_identifier = terminal(lexer_grammar::match_id_type::PARSER_IDENTIFIER)->*match_id_type::PARSER_IDENTIFIER;

                    const auto string_set = terminal(lexer_grammar::match_id_type::STRING_SET)->*match_id_type::STRING_SET;

                    const auto string = (terminal(lexer_grammar::match_id_type::SINGLE_CHARACTER_STRING) | lexer_grammar::match_id_type::STRING)->*match_id_type::STRING;

                    const auto term 
                        = optional_group
                        | repetition_group
                        | group
                        | lexer_identifier
                        | parser_identifier
                        | string_set
                        | string;

                    const auto integer = terminal(lexer_grammar::match_id_type::INTEGER)->*match_id_type::INTEGER;

                    const auto optional_term = (term >> lexer_grammar::match_id_type::OPTIONAL_OPERATOR)->*match_id_type::OPTIONAL;

                    const auto loop_0_term = (term >> lexer_grammar::match_id_type::LOOP_0_OPERATOR)->*match_id_type::LOOP_0;

                    const auto loop_1_term = (term >> lexer_grammar::match_id_type::LOOP_1_OPERATOR)->*match_id_type::LOOP_1;

                    const auto exclusion = (term >> lexer_grammar::match_id_type::EXCLUSION_OPERATOR >> term)->*match_id_type::EXCLUSION;

                    const auto multiplication = (integer >> -terminal(lexer_grammar::match_id_type::LOOP_0_OPERATOR) >> term)->*match_id_type::MULTIPLICATION;

                    const auto range_character = terminal(lexer_grammar::match_id_type::SINGLE_CHARACTER_STRING)->*match_id_type::RANGE_CHARACTER;

                    const auto range = (range_character >> lexer_grammar::match_id_type::RANGE_OPERATOR >> range_character)->*match_id_type::RANGE;

                    const auto factor 
                        = optional_term
                        | loop_0_term
                        | loop_1_term
                        | exclusion
                        | multiplication
                        | range
                        | term;

                    const auto concatenation 
                        = (factor >> +(-terminal(lexer_grammar::match_id_type::CONCATENATION_OPERATOR) >> factor))->*match_id_type::CONCATENATION
                        | factor;

                    const auto alternation 
                        = (concatenation >> +(lexer_grammar::match_id_type::ALTERNATION_OPERATOR >> concatenation))->*match_id_type::ALTERNATION
                        | concatenation;

                    m_expression = alternation;

                    const auto rule_definition_operator = terminal(lexer_grammar::match_id_type::DEFINITION_OPERATOR);

                    const auto terminator 
                        = terminal(lexer_grammar::match_id_type::NEWLINE_TERMINATOR)
                        | lexer_grammar::match_id_type::TERMINATOR
                        | end();

                    const auto lexer_rule = (lexer_identifier >> rule_definition_operator >> m_expression >> +terminator)->*match_id_type::LEXER_RULE;

                    const auto parser_rule = (parser_identifier >> rule_definition_operator >> m_expression >> +terminator)->*match_id_type::PARSER_RULE;

                    const auto rule 
                        = lexer_rule 
                        | parser_rule;

                    m_grammar = *rule;
                }

                parse_result parse(ParseContext& pc) noexcept {
                    return m_grammar.parse(pc);
                }

            private:
                rule<ParseContext> m_expression;
                rule<ParseContext> m_grammar;
            };
        };
    };


    /**
     * Returns the name of the given match id.
     * @param id the match id.
     * @return the name of the match id.
     */
    inline const char* get_id_name(ebnf_parser::lexer_grammar::match_id_type id) noexcept {
        switch (id) {
            case ebnf_parser::lexer_grammar::match_id_type::NEWLINE_TERMINATOR:
                return "NEWLINE_TERMINATOR";
            case ebnf_parser::lexer_grammar::match_id_type::LEXER_IDENTIFIER:
                return "LEXER_IDENTIFIER";
            case ebnf_parser::lexer_grammar::match_id_type::PARSER_IDENTIFIER:
                return "PARSER_IDENTIFIER";
            case ebnf_parser::lexer_grammar::match_id_type::STRING_SET:
                return "STRING_SET";
            case ebnf_parser::lexer_grammar::match_id_type::SINGLE_CHARACTER_STRING:
                return "SINGLE_CHARACTER_STRING";
            case ebnf_parser::lexer_grammar::match_id_type::STRING:
                return "STRING";
            case ebnf_parser::lexer_grammar::match_id_type::INTEGER:
                return "INTEGER";
            case ebnf_parser::lexer_grammar::match_id_type::RANGE_OPERATOR:
                return "RANGE_OPERATOR";
            case ebnf_parser::lexer_grammar::match_id_type::GROUP_START:
                return "GROUP_START";
            case ebnf_parser::lexer_grammar::match_id_type::GROUP_END:
                return "GROUP_END";
            case ebnf_parser::lexer_grammar::match_id_type::OPTIONAL_START:
                return "OPTIONAL_START";
            case ebnf_parser::lexer_grammar::match_id_type::OPTIONAL_END:
                return "OPTIONAL_END";
            case ebnf_parser::lexer_grammar::match_id_type::REPETITION_START:
                return "REPETITION_START";
            case ebnf_parser::lexer_grammar::match_id_type::REPETITION_END:
                return "REPETITION_END";
            case ebnf_parser::lexer_grammar::match_id_type::OPTIONAL_OPERATOR:
                return "OPTIONAL_OPERATOR";
            case ebnf_parser::lexer_grammar::match_id_type::LOOP_0_OPERATOR:
                return "LOOP_0_OPERATOR";
            case ebnf_parser::lexer_grammar::match_id_type::LOOP_1_OPERATOR:
                return "LOOP_1_OPERATOR";
            case ebnf_parser::lexer_grammar::match_id_type::CONCATENATION_OPERATOR:
                return "CONCATENATION_OPERATOR";
            case ebnf_parser::lexer_grammar::match_id_type::ALTERNATION_OPERATOR:
                return "ALTERNATION_OPERATOR";
            case ebnf_parser::lexer_grammar::match_id_type::EXCLUSION_OPERATOR:
                return "EXCLUSION_OPERATOR";
            case ebnf_parser::lexer_grammar::match_id_type::DEFINITION_OPERATOR:
                return "DEFINITION_OPERATOR";
            case ebnf_parser::lexer_grammar::match_id_type::TERMINATOR:
                return "TERMINATOR";
        }

        return "ebnf_parser: lexer_grammar: get_id_name: invalid id";
    }

    /**
     * Returns the name of the given match id.
     * @param id the match id.
     * @return the name of the match id.
     */
    inline const char* get_id_name(ebnf_parser::parser_grammar::match_id_type id) noexcept {
        switch (id) {
            case ebnf_parser::parser_grammar::match_id_type::LEXER_RULE:
                return "LEXER_RULE";
            case ebnf_parser::parser_grammar::match_id_type::PARSER_RULE:
                return "PARSER_RULE";
            case ebnf_parser::parser_grammar::match_id_type::ALTERNATION:
                return "ALTERNATION";
            case ebnf_parser::parser_grammar::match_id_type::CONCATENATION:
                return "CONCATENATION";
            case ebnf_parser::parser_grammar::match_id_type::OPTIONAL:
                return "OPTIONAL";
            case ebnf_parser::parser_grammar::match_id_type::LOOP_0:
                return "LOOP_0";
            case ebnf_parser::parser_grammar::match_id_type::LOOP_1:
                return "LOOP_1";
            case ebnf_parser::parser_grammar::match_id_type::EXCLUSION:
                return "EXCLUSION";
            case ebnf_parser::parser_grammar::match_id_type::MULTIPLICATION:
                return "MULTIPLICATION";
            case ebnf_parser::parser_grammar::match_id_type::GROUP:
                return "GROUP";
            case ebnf_parser::parser_grammar::match_id_type::RANGE:
                return "RANGE";
            case ebnf_parser::parser_grammar::match_id_type::LEXER_IDENTIFIER:
                return "LEXER_IDENTIFIER";
            case ebnf_parser::parser_grammar::match_id_type::PARSER_IDENTIFIER:
                return "PARSER_IDENTIFIER";
            case ebnf_parser::parser_grammar::match_id_type::STRING_SET:
                return "STRING_SET";
            case ebnf_parser::parser_grammar::match_id_type::STRING:
                return "STRING";
            case ebnf_parser::parser_grammar::match_id_type::RANGE_CHARACTER:
                return "RANGE_CHARACTER";
            case ebnf_parser::parser_grammar::match_id_type::INTEGER:
                return "INTEGER";
        }

        return "ebnf_parser: parser_grammar: get_id_name: invalid id";
    }


} //namespace parserlib


#endif //PARSERLIB_LANGUAGES_EBNF_PARSER_HPP
