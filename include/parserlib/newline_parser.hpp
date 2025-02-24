#ifndef PARSERLIB_NEWLINE_PARSER_HPP
#define PARSERLIB_NEWLINE_PARSER_HPP


#include "parser.hpp"


namespace parserlib {


    /**
     * A parser that increments the current line and sets the current column to the start column,
     * if the internal parser parses successfully.
     * The input iterator type must support the function `increment_line()` for the code to compile successfully.
     * @param Parser the internal parser.
     */
    template <class Parser>
    class newline_parser : public parser<newline_parser<Parser>> {
    public:
        /**
         * The constructor.
         * @param parser the internal parser.
         */
        newline_parser(const Parser& parser) noexcept
            : m_parser(parser)
        {
        }

        /**
         * Invokes the internal parser for parsing,
         * and if that succeeds, increments the line on the parse context.
         * @param context the parse context.
         * @return true if the internal parser parses successfully, false otherwise.
         */
        template <class ParseContext>
        bool parse(ParseContext& context) const {
            if (m_parser.parse(context)) {
                context.increment_line();
                return true;
            }
            return false;
        }

        /**
         * Invokes the internal parser for parsing,
         * and if that succeeds, increments the line on the parse context,
         * in the context of left recursion start.
         * @param context the parse context.
         * @return true if the internal parser parses successfully, false otherwise.
         */
        template <class ParseContext>
        bool parse_left_recursion_start(ParseContext& context) const {
            if (m_parser.parse_left_recursion_start(context)) {
                context.increment_line();
                return true;
            }
            return false;
        }

        /**
         * Invokes the internal parser for parsing,
         * and if that succeeds, increments the line on the parse context,
         * in the context of left recursion continuation.
         * @param context the parse context.
         * @param match_start_state parse state that signifies the start of a match.
         * @return true if the internal parser parses successfully, false otherwise.
         */
        template <class ParseContext>
        bool parse_left_recursion_continuation(ParseContext& context, const typename ParseContext::state& match_start_state) const {
            if (m_parser.parse_left_recursion_continuation(context, context.get_state())) {
                context.increment_line();
                return true;
            }
        }

    private:
        Parser m_parser;
    };


    template <class Parser>
    newline_parser<parser_wrapper_type<Parser>> newline(const Parser& parser) noexcept {
        return get_parser_wrapper(parser);
    }


} //namespace parserlib


#endif //PARSERLIB_NEWLINE_PARSER_HPP
