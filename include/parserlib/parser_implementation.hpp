#ifndef PARSERLIB_PARSER_IMPLEMENTATION_HPP
#define PARSERLIB_PARSER_IMPLEMENTATION_HPP


#include "parser_interface.hpp"


namespace parserlib {


    /**
     * Implementation class for parser_interface.
     * Used for invoking a parser through a set of virtual methods.
     * Used by `rule<ParseContext>` in order to allow recursive grammars.
     * @param ParseContext the parse context.
     * @param Parser internal parser.
     */
    template <class ParseContext, class Parser>
    class parser_implementation : public parser_interface<ParseContext> {
    public:
        /**
         * The constructor.
         * @param parser the internal parser.
         */
        parser_implementation(const Parser& parser)
            : m_parser(parser)
        {
        }

        /**
         * Passes the call to the internal parser.
         * @param context the parse context.
         * @return whatever the parser returns.
         */
        bool parse(ParseContext& context) const override {
            return m_parser.parse(context);
        }

        /**
         * Passes the call to the internal parser.
         * @param context the parse context.
         * @return whatever the parser returns.
         */
        bool parse_left_recursion_start(ParseContext& context) const override {
            return m_parser.parse_left_recursion_start(context);
        }

        /**
         * Passes the call to the internal parser.
         * @param context the parse context.
         * @param match_start_state parse state that signifies the start of a match.
         * @return whatever the parser returns.
         */
        bool parse_left_recursion_continuation(ParseContext& context, const typename ParseContext::state& match_start_state) const override {
            return m_parser.parse_left_recursion_continuation(context, match_start_state);
        }

    private:
        const Parser m_parser;
    };


} //namespace parserlib


#endif //PARSERLIB_PARSER_IMPLEMENTATION_HPP
