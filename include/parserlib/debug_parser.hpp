#ifndef PARSERLIB_DEBUG_PARSER_HPP
#define PARSERLIB_DEBUG_PARSER_HPP


#include "parser.hpp"


namespace parserlib {


    /**
     * A parser that can be used for debugging.
     * @param Parser the internal parser.
     */
    template <class Parser>
    class debug_parser : public parser<debug_parser<Parser>> {
    public:
        /**
         * The constructor.
         * @param parser the internal parser.
         */
        debug_parser(const Parser& parser)
            : m_parser(parser)
        {
        }

        /**
         * Non-left recursion parsing.
         * @param context parse context.
         * @return what the internal parser returns.
         */
        template <class ParseContext>
        bool parse(ParseContext& context) const {
            return do_parse(context, [&]() { return m_parser.parse(context); });
        }

        /**
         * Left recursion start parsing.
         * @param context parse context.
         * @return what the internal parser returns.
         */
        template <class ParseContext>
        bool parse_left_recursion_start(ParseContext& context) const {
            return do_parse(context, [&]() { return m_parser.parse_left_recursion_start(context); });
        }

        /**
         * Left recursion continuation parsing.
         * @param context parse context.
         * @param match_start_state parse state that signifies the start of a match.
         * @return what the internal parser returns.
         */
        template <class ParseContext>
        bool parse_left_recursion_continuation(ParseContext& context, const typename ParseContext::state& match_start_state) const {
            return do_parse(context, [&]() { return m_parser.parse_left_recursion_continuation(context, match_start_state); });
        }

    private:
        Parser m_parser;

        //put the breakpoint here for success
        template <class Context>
        void debug_success(Context& context) const {
        }

        //put the breakpoint here for failure
        template <class Context>
        void debug_failure(Context& context) const {
        }

        template <class Context, class ParseFunc>
        bool do_parse(Context& context, const ParseFunc& parse_func) const {
            if (parse_func()) {
                debug_success(context);
                return true;
            }
            debug_failure(context);
            return false;
        }
    };


    /**
     * Creates a debug parser.
     * @param parser parser to debug.
     * @return a debug parser.
     */
    template <class Parser>
    debug_parser<Parser> debug(const Parser& parser) {
        return { parser };
    }


} //namespace parserlib


#endif //PARSERLIB_DEBUG_PARSER_HPP
