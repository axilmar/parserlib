#ifndef PARSERLIB_BINARY_PARSE_NODES_HPP
#define PARSERLIB_BINARY_PARSE_NODES_HPP


#include <tuple>
#include <utility>
#include "parse_node.hpp"


namespace parserlib {


    class sequence_parse_node_base {};


    template <class... Parsers>
    class sequence_parse_node : public parse_node<sequence_parse_node<Parsers...>>, public sequence_parse_node_base {
        using tuple_type = std::tuple<Parsers...>;
    public:
        sequence_parse_node(const tuple_type& parsers) noexcept
            : m_parsers(parsers)
        {
        }

        template <class ParseContext>
        parse_result parse(ParseContext& pc) const noexcept {
            return parse_loop<0>(pc.state(), pc);
        }

        template <class ParseContext>
        parse_result parse_left_recursion_start(ParseContext& pc) const noexcept {
            return parse_left_recursion_start_loop(pc.state(), pc);
        }

        template <class ParseContext, class State>
        parse_result parse_left_recursion_continuation(ParseContext& pc, const State& match_start) const noexcept {
            return parse_left_recursion_continuation_loop(pc.state(), pc, match_start);
        }

        const tuple_type& parsers() const noexcept {
            return m_parsers;
        }

    private:
        tuple_type m_parsers;

        template <std::size_t I, class State, class ParseContext>
        parse_result parse_loop(const State& initial_state, ParseContext& pc) const noexcept {
            if constexpr (I < sizeof...(Parsers)) {
                parse_result result = std::get<I>(m_parsers).parse(pc);
                if (!result) {
                    pc.set_state(initial_state);
                    return result;
                }
                return parse_loop<I + 1>(initial_state, pc);
            }
            else {
                return true;
            }
        }

        template <class State, class ParseContext>
        parse_result parse_left_recursion_start_loop(const State& initial_state, ParseContext& pc) const noexcept {
            parse_result result = std::get<0>(m_parsers).parse_left_recursion_start(pc);
            if (!result) {
                pc.set_state(initial_state);
                return result;
            }
            return parse_loop<1>(initial_state, pc);
        }

        template <class State, class ParseContext>
        parse_result parse_left_recursion_continuation_loop(const State& initial_state, ParseContext& pc, const State& match_start) const noexcept {
            parse_result result = std::get<0>(m_parsers).parse_left_recursion_continuation(pc, pc.state());
            if (!result) {
                pc.set_state(initial_state);
                return result;
            }
            return parse_loop<1>(initial_state, pc);
        }
    };


    template <class L, class R, std::enable_if_t<(std::is_base_of_v<parse_node_base, std::decay_t<L>> || std::is_base_of_v<parse_node_base, std::decay_t<R>>) && !std::is_base_of_v<sequence_parse_node_base, L> && !std::is_base_of_v<sequence_parse_node_base, R>, bool> = true>
    auto operator >> (L&& l, R&& r) noexcept {
        return sequence_parse_node(std::make_tuple(get_parse_node_wrapper(std::forward<L>(l)), get_parse_node_wrapper(std::forward<R>(r))));
    }


    template <class... L, class... R>
    auto operator >> (const sequence_parse_node<L...>& l, const sequence_parse_node<R...>& r) noexcept {
        return sequence_parse_node(std::tuple_cat(l.parsers(), r.parsers()));
    }


    template <class... L, class R, std::enable_if_t<!std::is_base_of_v<sequence_parse_node_base, R>, bool> = true>
    auto operator >> (const sequence_parse_node<L...>& l, R&& r) noexcept {
        return sequence_parse_node(std::tuple_cat(l.parsers(), std::make_tuple(get_parse_node_wrapper(std::forward<R>(r)))));
    }


    template <class L, class... R, std::enable_if_t<!std::is_base_of_v<sequence_parse_node_base, L>, bool> = true>
    auto operator >> (L&& l, const sequence_parse_node<R...>& r) noexcept {
        return sequence_parse_node(std::tuple_cat(std::make_tuple(get_parse_node_wrapper(std::forward<L>(l))), r.parsers()));
    }


    class choice_parse_node_base {};


    template <class... Parsers>
    class choice_parse_node : public parse_node<choice_parse_node<Parsers...>>, public choice_parse_node_base {
        using tuple_type = std::tuple<Parsers...>;
    public:
        choice_parse_node(const tuple_type& parsers) noexcept
            : m_parsers(parsers)
        {
        }

        template <class ParseContext>
        parse_result parse(ParseContext& pc) const noexcept {
            return parse_loop<0>(pc.state(), pc);
        }

        template <class ParseContext>
        parse_result parse_left_recursion_start(ParseContext& pc) const noexcept {
            return parse_left_recursion_start_loop<0>(pc.state(), pc);
        }

        template <class ParseContext, class State>
        parse_result parse_left_recursion_continuation(ParseContext& pc, const State& match_start) const noexcept {
            return parse_left_recursion_continuation_loop<0>(pc.state(), pc, match_start);
        }

        const tuple_type& parsers() const noexcept {
            return m_parsers;
        }

    private:
        tuple_type m_parsers;

        template <std::size_t I, class State, class ParseContext>
        parse_result parse_loop(const State& initial_state, ParseContext& pc) const noexcept {
            if constexpr (I < sizeof...(Parsers)) {
                parse_result result = std::get<I>(m_parsers).parse(pc);
                switch (result.value()) {
                    case parse_result::TRUE:
                    case parse_result::LEFT_RECURSION:
                        return result;
                }
                pc.set_state(initial_state);
                return parse_loop<I + 1>(initial_state, pc);
            }
            else {
                pc.set_state(initial_state);
                return false;
            }
        }

        template <std::size_t I, class State, class ParseContext>
        parse_result parse_left_recursion_start_loop(const State& initial_state, ParseContext& pc) const noexcept {
            if constexpr (I < sizeof...(Parsers)) {
                parse_result result = std::get<I>(m_parsers).parse_left_recursion_start(pc);
                switch (result.value()) {
                    case parse_result::TRUE:
                    case parse_result::LEFT_RECURSION:
                        return result;
                }
                pc.set_state(initial_state);
                return parse_left_recursion_start_loop<I + 1>(initial_state, pc);
            }
            else {
                pc.set_state(initial_state);
                return false;
            }
        }

        template <std::size_t I, class State, class ParseContext>
        parse_result parse_left_recursion_continuation_loop(const State& initial_state, ParseContext& pc, const State& match_start) const noexcept {
            if constexpr (I < sizeof...(Parsers)) {
                parse_result result = std::get<I>(m_parsers).parse_left_recursion_continuation(pc, match_start);
                switch (result.value()) {
                    case parse_result::TRUE:
                    case parse_result::LEFT_RECURSION:
                        return result;
                }
                pc.set_state(initial_state);
                return parse_left_recursion_continuation_loop<I + 1>(initial_state, pc, match_start);
            }
            else {
                pc.set_state(initial_state);
                return false;
            }
        }
    };


    template <class L, class R, std::enable_if_t<(std::is_base_of_v<parse_node_base, std::decay_t<L>> || std::is_base_of_v<parse_node_base, std::decay_t<R>>) && !std::is_base_of_v<choice_parse_node_base, L> && !std::is_base_of_v<choice_parse_node_base, R>, bool> = true>
    auto operator | (L&& l, R&& r) noexcept {
        return choice_parse_node(std::make_tuple(get_parse_node_wrapper(std::forward<L>(l)), get_parse_node_wrapper(std::forward<R>(r))));
    }


    template <class... L, class... R>
    auto operator | (const choice_parse_node<L...>& l, const choice_parse_node<R...>& r) noexcept {
        return choice_parse_node(std::tuple_cat(l.parsers(), r.parsers()));
    }


    template <class... L, class R, std::enable_if_t<!std::is_base_of_v<choice_parse_node_base, R>, bool> = true>
    auto operator | (const choice_parse_node<L...>& l, R&& r) noexcept {
        return choice_parse_node(std::tuple_cat(l.parsers(), std::make_tuple(get_parse_node_wrapper(std::forward<R>(r)))));
    }


    template <class L, class... R, std::enable_if_t<!std::is_base_of_v<choice_parse_node_base, L>, bool> = true>
    auto operator | (L&& l, const choice_parse_node<R...>& r) noexcept {
        return choice_parse_node(std::tuple_cat(std::make_tuple(get_parse_node_wrapper(std::forward<L>(l))), r.parsers()));
    }


    template <class L, class R, std::enable_if_t<std::is_base_of_v<parse_node_base, std::decay_t<L>> || std::is_base_of_v<parse_node_base, std::decay_t<R>>, bool> = true>
    auto operator - (L&& l, R&& r) noexcept {
        return !get_parser_wrapper(r), get_parse_node_wrapper(std::forward<L>(l));
    }


    template <class Parser, class MatchId>
    class match_parser_node : public parse_node<match_parser_node<Parser, MatchId>> {
    public:
        match_parser_node(const Parser& parser, const MatchId& match_id) noexcept
            : m_parser(parser)
            , m_match_id(match_id)
        {
        }

        template <class ParseContext>
        parse_result parse(ParseContext& pc) const noexcept {
            const auto match_start = pc.state();
            parse_result result = m_parser.parse(pc);
            if (result) {
                pc.add_match(m_match_id, match_start.position(), pc.parse_position(), match_start.match_count());
            }
            return result;
        }

        template <class ParseContext>
        parse_result parse_left_recursion_start(ParseContext& pc) const noexcept {
            const auto match_start = pc.state();
            parse_result result = m_parser.parse_left_recursion_start(pc);
            if (result) {
                pc.add_match(m_match_id, match_start.position(), pc.parse_position(), match_start.match_count());
            }
            return result;
        }

        template <class ParseContext, class State>
        parse_result parse_left_recursion_continuation(ParseContext& pc, const State& match_start) const noexcept {
            parse_result result = m_parser.parse_left_recursion_continuation(pc, pc.state());
            if (result) {
                pc.add_match(m_match_id, match_start.position(), pc.parse_position(), match_start.match_count());
            }
            return result;
        }

    private:
        Parser m_parser;
        MatchId m_match_id;
    };


    template <class T, class MatchId>
    auto operator ->* (T&& t, const MatchId& match_id) noexcept {
        return match_parser_node(get_parse_node_wrapper(std::forward<T>(t)), match_id);
    }


} //namespace parserlib


#endif //PARSERLIB_BINARY_PARSE_NODES_HPP
