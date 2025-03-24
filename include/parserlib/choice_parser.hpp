#ifndef PARSERLIB_CHOICE_PARSER_HPP
#define PARSERLIB_CHOICE_PARSER_HPP


#include <cstddef>
#include <tuple>
#include <iostream>
#include "parser.hpp"


namespace parserlib {


    /**
     * A parser that invokes each parser, from a series of parsers,
     * until a parser succeeds.
     * @param Parsers series of parsers to invoke.
     */
    template <class... Parsers>
    class choice_parser : public parser<choice_parser<Parsers...>> {
    public:
        /** Tuple of parsers. */
        using tuple_type = std::tuple<Parsers...>;

        /**
         * The constructor.
         * @param parsers parsers to use.
         */
        choice_parser(const tuple_type& parsers)
            : m_parsers(parsers)
        {
        }

        /**
         * Invokes the parsers one at a time, until one succeeds.
         * The parse context is restored between two invocations.
         * @param context the context to use.
         * @return true if parsing suceeds, false otherwise.
         */
        template <class ParseContext>
        bool parse(ParseContext& context) const {
            return parse_loop<0>(context);
        }

        /**
         * Invokes the parsers in the context of left recursion parsing,
         * when a non-left-recursive part of the grammar must parse successfully.
         * @param context the context to use.
         * @return true if parsing suceeds, false otherwise.
         */
        template <class ParseContext>
        bool parse_left_recursion_start(ParseContext& context) const {
            return parse_left_recursion_start_loop<0>(context);
        }

        /**
         * Invokes the parsers in the context of left recursion parsing,
         * when the left-recursive part of the grammar must parse successfully.
         * @param context the context to use.
         * @param match_start_state parse state that signifies the start of a match.
         * @return true if parsing suceeds, false otherwise.
         */
        template <class ParseContext>
        bool parse_left_recursion_continuation(ParseContext& context, const typename ParseContext::state& match_start_state) const {
            return parse_left_recursion_continuation_loop<0>(context, match_start_state);
        }

        /**
         * Returns the parsers.
         * @return the parsers.
         */
        const tuple_type& parsers() const {
            return m_parsers;
        }

    private:
        const tuple_type m_parsers;

        template <std::size_t Index, class ParseContext>
        bool parse_loop(ParseContext& context) const {
            if constexpr (Index < std::tuple_size_v<tuple_type>) {
                return std::get<Index>(m_parsers).parse(context) || parse_loop<Index + 1>(context);
            }
            else {
                return false;
            }
        }

        template <std::size_t Index, class ParseContext>
        bool parse_left_recursion_start_loop(ParseContext& context) const {
            if constexpr (Index < std::tuple_size_v<tuple_type>) {
                return std::get<Index>(m_parsers).parse_left_recursion_start(context) || parse_left_recursion_start_loop<Index + 1>(context);
            }
            else {
                return false;
            }
        }

        template <std::size_t Index, class ParseContext>
        bool parse_left_recursion_continuation_loop(ParseContext& context, const typename ParseContext::state& match_start_state) const {
            if constexpr (Index < std::tuple_size_v<tuple_type>) {
                return std::get<Index>(m_parsers).parse_left_recursion_continuation(context, match_start_state) || parse_left_recursion_continuation_loop<Index + 1>(context, match_start_state);
            }
            else {
                return false;
            }
        }
    };


    /**
     * Creates a choice of parsers between two parsers.
     * Either parser can be a terminal.
     * The appropriate parser wrapper is created for each given parser.
     * One of the two parsers shall be a parser object.
     * @param l the left parser.
     * @param r the right parser.
     * @return the choice of parsers.
     */
    template <class L, class R, std::enable_if_t<std::is_base_of_v<parser<L>, L> || std::is_base_of_v<parser<R>, R>, bool> = true>
    choice_parser<parser_wrapper_type<L>, parser_wrapper_type<R>> operator | (const L& l, const R& r) {
        return { std::make_tuple(get_parser_wrapper(l), get_parser_wrapper(r)) };
    }


    /**
     * Adds a parser to a choice of parsers.
     * @param l the choice parser.
     * @param r the parser.
     * @return the choice of parsers that contains all the parsers of the left object and the parser of the right object.
     */
    template <class... LeftParsers, class R>
    choice_parser<LeftParsers..., parser_wrapper_type<R>> operator | (const choice_parser<LeftParsers...>& l, const R& r) {
        return { std::tuple_cat(l.parsers(), std::make_tuple(get_parser_wrapper(r))) };
    }


    /**
     * Concatenates a parser with a choice of parsers.
     * @param l the parser.
     * @param r the choice parser.
     * @return  the parser of the left object and the choice of parsers that contains all the parsers of the left object.
     */
    template <class L, class... RightParsers>
    choice_parser<parser_wrapper_type<L>, RightParsers...> operator | (const L& l, const choice_parser<RightParsers...>& r) {
        return { std::tuple_cat(std::make_tuple(get_parser_wrapper(l)), r.parsers()) };
    };


    /**
     * Concatenates two choice parsers.
     * @param l the left choice parser.
     * @param r the right choice parser.
     * @return a choice parser that contains the parsers of the left and the parsers of the right parsers.
     */
    template <class... LeftParsers, class... RightParsers>
    choice_parser<LeftParsers..., RightParsers...> operator | (const choice_parser<LeftParsers...>& l, const choice_parser<RightParsers...>& r) {
        return { std::tuple_cat(l.parsers(), r.parsers()) };
    };


} //namespace parserlib


#endif //PARSERLIB_CHOICE_PARSER_HPP
