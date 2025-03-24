#ifndef PARSERLIB_N_TIMES_PARSER_HPP
#define PARSERLIB_N_TIMES_PARSER_HPP


#include <cassert>
#include "parser.hpp"


namespace parserlib {


    /**
     * A parser that repeats the internal parser a specific number of times.
     * @param Parser the internal parser.
     */
    template <class Parser>
    class n_times_parser : public parser<n_times_parser<Parser>> {
    public:
        /**
         * The constructor.
         * @param parser the internal parser.
         * @param count number of times to repeat the parser.
         */
        n_times_parser(const Parser& parser, std::size_t count)
            : m_parser(parser)
            , m_count(count)
        {
            assert(count > 0);
        }

        /**
         * Invokes the internal parser in a loop .
         * @param context parse context.
         * @return true if the internal parser parses successfully at every loop step, false otherwise.
         */
        template <class ParseContext>
        bool parse(ParseContext& context) const {
            auto state = context.get_state();
            for(std::size_t counter = 0; counter < m_count; ++counter) {
                if (!m_parser.parse(context)) {
                    context.set_state(state);
                    return false;
                }
            }
            return true;
        }

        /**
         * Invokes the internal parser in a loop,
         * in the context of left recursion start.
         * @param context parse context.
         * @return true if the internal parser parses successfully at every loop step, false otherwise.
         */
        template <class ParseContext>
        bool parse_left_recursion_start(ParseContext& context) const {
            auto state = context.get_state();
            for (std::size_t counter = 0; counter < m_count; ++counter) {
                if (!m_parser.parse_left_recursion_start(context)) {
                    context.set_state(state);
                    return false;
                }
            }
            return true;
        }

        /**
         * Invokes the internal parser in a loop,
         * in the context of left recursion continuation.
         * @param context parse context.
         * @param match_start_state parse state that signifies the start of a match.
         * @return true if the internal parser parses successfully at every loop step, false otherwise.
         */
        template <class ParseContext>
        bool parse_left_recursion_continuation(ParseContext& context, const typename ParseContext::state& match_start_state) const {
            auto state = context.get_state();
            for (std::size_t counter = 0; counter < m_count; ++counter) {
                if (!m_parser.parse_left_recursion_continuation(context, context.get_state())) {
                    context.set_state(state);
                    return false;
                }
            }
            return true;
        }

    private:
        const Parser m_parser;
        const std::size_t m_count;
    };


} //namespace parserlib


#endif //PARSERLIB_N_TIMES_PARSER_HPP
