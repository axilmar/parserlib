#ifndef PARSERLIB_CHOICE_PARSER_HPP
#define PARSERLIB_CHOICE_PARSER_HPP


#include <cstddef>
#include <tuple>
#include <iostream>
#include "parser.hpp"


namespace parserlib {


    template <class... Parsers>
    class choice_parser : public parser<choice_parser<Parsers...>> {
    public:
        using tuple_type = std::tuple<Parsers...>;

        choice_parser(const tuple_type& parsers)
            : m_parsers(parsers)
        {
        }

        template <class ParseContext>
        bool parse(ParseContext& context) const noexcept {
            return parse_loop<0>(context);
        }

        template <class ParseContext>
        bool parse_left_recursion_start(ParseContext& context) const noexcept {
            return parse_left_recursion_start_loop<0>(context);
        }

        template <class ParseContext>
        bool parse_left_recursion_continuation(ParseContext& context, const typename ParseContext::state& match_start_state) const noexcept {
            return parse_left_recursion_continuation_loop<0>(context, match_start_state);
        }

        const tuple_type& parsers() const noexcept {
            return m_parsers;
        }

    private:
        const tuple_type m_parsers;

        template <std::size_t Index, class ParseContext>
        bool parse_loop(ParseContext& context) const noexcept {
            if constexpr (Index < std::tuple_size_v<tuple_type>) {
                return std::get<Index>(m_parsers).parse(context) || parse_loop<Index + 1>(context);
            }
            else {
                return false;
            }
        }

        template <std::size_t Index, class ParseContext>
        bool parse_left_recursion_start_loop(ParseContext& context) const noexcept {
            if constexpr (Index < std::tuple_size_v<tuple_type>) {
                return std::get<Index>(m_parsers).parse_left_recursion_start(context) || parse_left_recursion_start_loop<Index + 1>(context);
            }
            else {
                return false;
            }
        }

        template <std::size_t Index, class ParseContext>
        bool parse_left_recursion_continuation_loop(ParseContext& context, const typename ParseContext::state& match_start_state) const noexcept {
            if constexpr (Index < std::tuple_size_v<tuple_type>) {
                return std::get<Index>(m_parsers).parse_left_recursion_continuation(context, match_start_state) || parse_left_recursion_continuation_loop<Index + 1>(context, match_start_state);
            }
            else {
                return false;
            }
        }
    };


    template <class L, class R>
    choice_parser<parser_wrapper_type<L>, parser_wrapper_type<R>> operator | (const L& l, const R& r) noexcept {
        return { std::make_tuple(get_parser_wrapper(l), get_parser_wrapper(r)) };
    }


    template <class... LeftParsers, class R>
    choice_parser<LeftParsers..., parser_wrapper_type<R>> operator | (const choice_parser<LeftParsers...>& l, const R& r) noexcept {
        return { std::tuple_cat(l.parsers(), std::make_tuple(get_parser_wrapper(r))) };
    }


    template <class L, class... RightParsers>
    choice_parser<parser_wrapper_type<L>, RightParsers...> operator | (const L& l, const choice_parser<RightParsers...>& r) noexcept {
        return { std::tuple_cat(std::make_tuple(get_parser_wrapper(l)), r.parsers()) };
    };


    template <class... LeftParsers, class... RightParsers>
    choice_parser<LeftParsers..., RightParsers...> operator | (const choice_parser<LeftParsers...>& l, const choice_parser<RightParsers...>& r) noexcept {
        return { std::tuple_cat(l.parsers(), r.parsers()) };
    };


} //namespace parserlib


#endif //PARSERLIB_CHOICE_PARSER_HPP
