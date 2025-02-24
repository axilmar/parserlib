#ifndef PARSERLIB_SEQUENCE_PARSER_HPP
#define PARSERLIB_SEQUENCE_PARSER_HPP


#include <cstddef>
#include <tuple>
#include "parser.hpp"


namespace parserlib {


    /**
     * A parser that uses a series of other parsers to parse.
     * All the parsers in the series must parse successfully in order for the sequence to parse successfully.
     * @param Parsers the sequence of parsers to parse.
     */
    template <class... Parsers>
    class sequence_parser : public parser<sequence_parser<Parsers...>> {
    public:
        /** Tuple of parsers. */
        using tuple_type = std::tuple<Parsers...>;

        /**
         * The constructor.
         * @param parsers parsers to use.
         */
        sequence_parser(const tuple_type& parsers)
            : m_parsers(parsers)
        {
        }

        /**
         * Invokes all the parsers in sequence.
         * If one of them fails, then the parse context state is restored to the one
         * before this call.
         * @param context the parse context.
         * @return true if all the parsers parsed successfully, false otherwise.
         */
        template <class ParseContext>
        bool parse(ParseContext& context) const {
            const auto state = context.get_state();
            if (parse_loop<0>(context)) {
                return true;
            }
            context.set_state(state);
            return false;
        }

        template <class ParseContext>
        bool parse_left_recursion_start(ParseContext& context) const {
            const auto state = context.get_state();
            if (parse_left_recursion_start_loop<0>(context)) {
                return true;
            }
            context.set_state(state);
            return false;
        }

        template <class ParseContext>
        bool parse_left_recursion_continuation(ParseContext& context, const typename ParseContext::state& match_start_state) const {
            const auto state = context.get_state();
            if (parse_left_recursion_continuation_loop<0>(context, state)) {
                return true;
            }
            context.set_state(state);
            return false;
        }

        const tuple_type& parsers() const noexcept {
            return m_parsers;
        }

    private:
        const tuple_type m_parsers;

        template <std::size_t Index, class ParseContext>
        bool parse_loop(ParseContext& context) const {
            if constexpr (Index < std::tuple_size_v<tuple_type>) {
                return std::get<Index>(m_parsers).parse(context) && parse_loop<Index + 1>(context);
            }
            else {
                return true;
            }
        }

        template <std::size_t Index, class ParseContext>
        bool parse_left_recursion_start_loop(ParseContext& context) const {
            if constexpr (Index < std::tuple_size_v<tuple_type>) {
                const auto start_parse_position = context.parse_position();
                if (std::get<Index>(m_parsers).parse_left_recursion_start(context)) {
                    if (context.parse_position() == start_parse_position && parse_left_recursion_start_loop<Index + 1>(context)) {
                        return true;
                    }
                    return parse_loop<Index + 1>(context);
                }
                return false;
            }
            else {
                return true;
            }
        }

        template <std::size_t Index, class ParseContext>
        bool parse_left_recursion_continuation_loop(ParseContext& context, const typename ParseContext::state& match_start_state) const {
            if constexpr (Index < std::tuple_size_v<tuple_type>) {
                const auto start_parse_position = context.parse_position();
                if (std::get<Index>(m_parsers).parse_left_recursion_continuation(context, context.get_state())) {
                    if (context.parse_position() == start_parse_position && parse_left_recursion_continuation_loop<Index + 1>(context, context.get_state())) {
                        return true;
                    }
                    return parse_loop<Index + 1>(context);
                }
                return false;
            }
            else {
                return true;
            }
        }
    };


    /**
     * Creates a sequence of parsers between two parsers.
     * Either parser can be a terminal.
     * The appropriate parser wrapper is created for each given parser.
     * One of the two parsers shall be a parser object.
     * @param l the left parser.
     * @param r the right parser.
     * @return the sequence of parsers.
     */
    template <class L, class R, std::enable_if_t<std::is_base_of_v<parser<L>, L> || std::is_base_of_v<parser<R>, R>, bool> = true>
    sequence_parser<parser_wrapper_type<L>, parser_wrapper_type<R>>  operator >> (const L& l, const R& r) noexcept {
        return { std::make_tuple(get_parser_wrapper(l), get_parser_wrapper(r)) };
    }


    /**
     * Adds a parser to a sequence of parsers.
     * @param l the sequence parser.
     * @param r the parser.
     * @return the sequence of parsers that contains all the parsers of the left object and the parser of the right object.
     */
    template <class... LeftParsers, class R>
    sequence_parser<LeftParsers..., parser_wrapper_type<R>>  operator >> (const sequence_parser<LeftParsers...>& l, const R& r) noexcept {
        return { std::tuple_cat(l.parsers(), std::make_tuple(get_parser_wrapper(r))) };
    }


    /**
     * Concatenates a parser with a sequence of parsers.
     * @param l the parser.
     * @param r the sequence parser.
     * @return  the parser of the left object and the sequence of parsers that contains all the parsers of the left object.
     */
    template <class L, class... RightParsers>
    sequence_parser<parser_wrapper_type<L>, RightParsers...>  operator >> (const L& l, const sequence_parser<RightParsers...>& r) noexcept {
        return { std::tuple_cat(std::make_tuple(get_parser_wrapper(l)), r.parsers()) };
    };


    /**
     * Concatenates two sequence parsers.
     * @param l the left sequence parser.
     * @param r the right sequence parser.
     * @return a sequence parser that contains the parsers of the left and the parsers of the right parsers.
     */
    template <class... LeftParsers, class... RightParsers>
    sequence_parser<LeftParsers..., RightParsers...>  operator >> (const sequence_parser<LeftParsers...>& l, const sequence_parser<RightParsers...>& r) noexcept {
        return { std::tuple_cat(l.parsers(), r.parsers()) };
    };


} //namespace parserlib


#endif //PARSERLIB_SEQUENCE_PARSER_HPP
