#ifndef PARSERLIB_LOGICAL_NOT_PARSER_HPP
#define PARSERLIB_LOGICAL_NOT_PARSER_HPP


#include "parser.hpp"


namespace parserlib {


    /**
     * A parser that tests another parser as a predicate.
     * The result of the parser to test is inverted.
     * After parsing, the context's state is restored to the state before the call.
     * @param Parser the parser to test.
     */
    template <class Parser>
    class logical_not_parser : public parser<logical_not_parser<Parser>> {
    public:
        /**
         * The constructor.
         * @param parser the parser to test.
         */
        logical_not_parser(const Parser& parser)
            : m_parser(parser)
        {
        }

        /**
         * Invokes the internal parser, then restores the context to the state before the call.
         * @param context parse context.
         * @return the opposite of the result of the internal parser.
         */
        template <class ParseContext>
        bool parse(ParseContext& context) const {
            const auto state = context.get_state();
            const bool result = !m_parser.parse(context);
            context.set_state(state);
            return result;
        }

        /**
         * Invokes the internal parser, in the context of left recursion start,
         * then restores the context to the state before the call.
         * @param context parse context.
         * @return the opposite of the result of the internal parser.
         */
        template <class ParseContext>
        bool parse_left_recursion_start(ParseContext& context) const {
            const auto state = context.get_state();
            const bool result = !m_parser.parse_left_recursion_start(context);
            context.set_state(state);
            return result;
        }

        /**
         * Invokes the internal parser, in the context of left recursion continuation,
         * then restores the context to the state before the call.
         * @param context parse context.
         * @param match_start_state parse state that signifies the start of a match.
         * @return the opposite of the result of the internal parser.
         */
        template <class ParseContext>
        bool parse_left_recursion_continuation(ParseContext& context, const typename ParseContext::state& match_start_state) const {
            const auto state = context.get_state();
            const bool result = !m_parser.parse_left_recursion_continuation(context, state);
            context.set_state(state);
            return result;
        }

    private:
        const Parser m_parser;
    };


    /**
     * Creates a sequence of parsers where the 2nd expression is turned into a negative predicate.
     * @param l the left parser.
     * @param r the right parser.
     * @return the sequence of not(the right parser) to the left parser.
     */
    template <class L, class R, std::enable_if_t<std::is_base_of_v<parser<L>, L> || std::is_base_of_v<parser<R>, R>, bool> = true>
    auto operator - (const L& l, const R& r) noexcept {
        return !get_parser_wrapper(r) >> get_parser_wrapper(l);
    }


} //namespace parserlib


#endif //PARSERLIB_LOGICAL_NOT_PARSER_HPP
