#ifndef PARSERLIB_LANGUAGES_EBNF_PARSER_HPP
#define PARSERLIB_LANGUAGES_EBNF_PARSER_HPP


#include "parserlib.hpp"


namespace parserlib {


    using namespace parserlib;


    /**
     * An EBNF parser.
     *
     * It provides the following grammar (expressed in EBNF):
     *
     *      ebnf = *rule;
     *
     *      rule = lexer_rule | basic_rule;
     *
     *      lexer_rule = '#', basic_rule;
     *
     *      basic_rule = identifier, rule_definition_operator, expression, terminator;
     *
     *      rule_definition_operator = '=' | ':' | '::=';
     *
     *      expression = alternation;
     *
     *      terminator = (';' | '.' | '\n\n')*;
     *
     *      alternation = concatenation, ('|', concatenation)*;
     *
     *      concatenation = factor, (','?, factor)*;
     *
     *      factor = term, '?'
     *             | term, '*'
     *             | term, '+'
     *             | term, '-', term
     *             | integer, '*', term
     *             | term;
     *
     *      term = '[', expression, ']'
     *           | '{', expression, '}'
     *           | '(', expression, ')'
     *           | character_string, '..', character_string
     *           | '%' string
     *           | identifier
     *           | string;
     *
     *      identifier = '<'?, letter, *(letter | digit | '_' | '-'), '>'?;
     *
     *      string = '"', (single_character_string - '"')*, '"'
     *             | "'", (single_character_string - "'")*, "'";
     *
     *      character_string = "'", (single_character_string - "'"), "'";
     *
     *      single_character_string = escaped_character | letter | digit | symbol;
     *
     *      escaped_character = '\\' | '\"' | '\'' | '\n' | '\r' | '\t' | '\v' | '\f' | '\b';
     *
     *      symbol = one of "`~!@#$%^&*()-_=+[{]}|:;<,>.?/";
     *
     *      integer = digit+;
     *
     * Comments start with '(*' and end with '*)'.
     *
     * Letter is any single_character_string for which the function 'std::isalpha' returns true for the current C locale.
     *
     * Digit is any single_character_string for which the function 'std::isdigit' returns true for the current C locale.
     *
     * The grammar is flexible enough to understand various versions of EBNF:
     *
     *  - identifiers can contain hyphens.
     *  - terminators can be a list of either a semicolon, or a dot, or two or more newline characters.
     *  - the concatenation operator is optional.
     *  - the rule definition operator can be one of '=', ':', '::='.
     *  - identifiers can optionally be enclosed in '<', '>'.
     *  - if an identifier does not represent a rule, then it is considered to be a terminal.
     *
     * Furthemore, it allows for the following extensions:
     *
     *  - lexer rules, using the prefix operator '#'.
     *    Lexer rules can later be used to create a lexer for the specific grammar.
     *    Example:
     *
     *      #identifier = letter (letter | digit | '_')*;
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
                IDENTIFIER,
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
                ZERO_OR_MORE_OPERATOR,
                ONE_OR_MORE_OPERATOR,
                SEQUENCE_OPERATOR,
                CHOICE_OPERATOR,
                EXCEPTION_OPERATOR,
                DEFINITION_OPERATOR,
                LEXER_OPERATOR,
                TERMINATOR
            };

            /** The error id type. */
            using error_id_type = parserlib::ebnf_parser::error_id_type;

            /**
             * Parses the given input and creates the parsed tokens.
             * @param pc the parse context.
             * @return the parsed result.
             */
            template <class ParseContext>
            parse_result parse(ParseContext& pc) const noexcept {
                const auto newline_terminator = terminal("\n\n")->*match_id_type::NEWLINE_TERMINATOR;

                const auto whitespace = +terminal(&isspace);

                const auto comment = terminal("(*") >> *(any() - "*)") >> "*)";

                const auto digit = terminal(&isdigit);
                const auto letter = terminal(&isalpha);
                const auto alnum = terminal(&isalnum);

                const auto identifier1 = (letter >> *(alnum | '_' | '-'))->*match_id_type::IDENTIFIER;
                const auto identifier = '<' >> identifier1 >> '>' | identifier1;

                const auto escaped_character_value = terminal("\\\\") | "\\\"" | "\\\'" | "\\n" | "\\r" | "\\t" | "\\v" | "\\f" | "\\b";

                const auto symbol = set("`~!@#$%^&*()-_=+[{]}|:;<,>.?/");

                const auto character_value = escaped_character_value | alnum | symbol;

                const auto string_set = '%' >> (*((character_value | "\\%") - '%'))->*match_id_type::STRING_SET >> '%';

                const auto single_character_string = ('\'' >> (character_value - '\'') >> '\'')->*match_id_type::SINGLE_CHARACTER_STRING;

                const auto string1 = (*(character_value - '"'))->*match_id_type::STRING;
                const auto string2 = (*(character_value - '\''))->*match_id_type::STRING;
                const auto string = '\"' >> string1 >> '\"' | '\'' >> string2 >> '\'';
                
                const auto integer = +digit;

                const auto range_operator = terminal("..")->*match_id_type::RANGE_OPERATOR;

                const auto group_start = terminal('(')->*match_id_type::GROUP_START;
                const auto group_end = terminal(')')->*match_id_type::GROUP_END;

                const auto optional_start = terminal('[')->*match_id_type::OPTIONAL_START;
                const auto optional_end = terminal(']')->*match_id_type::OPTIONAL_END;

                const auto repetition_start = terminal('{')->*match_id_type::REPETITION_START;
                const auto repetition_end = terminal('}')->*match_id_type::REPETITION_END;

                const auto optional_operator = terminal('?')->*match_id_type::OPTIONAL_OPERATOR;

                const auto zero_or_more_operator = terminal('*')->*match_id_type::ZERO_OR_MORE_OPERATOR;

                const auto one_or_more_operator = terminal('+')->*match_id_type::ONE_OR_MORE_OPERATOR;

                const auto sequence_operator = terminal(',')->*match_id_type::SEQUENCE_OPERATOR;

                const auto choice_operator = terminal('|')->*match_id_type::CHOICE_OPERATOR;

                const auto exception_operator = terminal('-')->*match_id_type::EXCEPTION_OPERATOR;

                const auto definition_operator = (terminal("::=") | '=' | ':')->*match_id_type::DEFINITION_OPERATOR;

                const auto lexer_operator = terminal('#')->*match_id_type::LEXER_OPERATOR;

                const auto terminator = set(";.")->*match_id_type::TERMINATOR;

                const auto token
                    = newline_terminator
                    | whitespace
                    | comment
                    | identifier
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
                    | zero_or_more_operator
                    | one_or_more_operator
                    | sequence_operator
                    | choice_operator
                    | exception_operator
                    | definition_operator
                    | lexer_operator
                    | terminator
                    | error(error_id_type::INVALID_CHARACTERS, skip_until(set("\n(\'\".[{?*,|-:=#;.%<") | alnum | whitespace));

                const auto grammar = *token;

                return grammar.parse(pc);
            }
        };

        //class parser_grammar {
        //};

        //template <class Source>
        //using parser_type = parser<Source, lexer_grammar, parser_grammar>;
    };


} //namespace parserlib


#endif //PARSERLIB_LANGUAGES_EBNF_PARSER_HPP
