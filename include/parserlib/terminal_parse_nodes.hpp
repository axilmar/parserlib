#ifndef PARSERLIB_TERMINAL_PARSE_NODES_HPP
#define PARSERLIB_TERMINAL_PARSE_NODES_HPP


#include <algorithm>
#include "parse_node.hpp"


namespace parserlib {


    template <class Terminal>
    class terminal_parse_node : public parse_node<terminal_parse_node<Terminal>> {
    public:
        terminal_parse_node(const Terminal& terminal)
            : m_terminal(terminal)
        {
        }

        template <class ParseContext>
        parse_result parse(ParseContext& pc) const noexcept {
            if (pc.is_valid_parse_position() && ParseContext::compare(*pc.parse_position(), m_terminal) == 0) {
                pc.increment_parse_position();
                return true;
            }
            return false;
        }

        template <class ParseContext>
        parse_result parse_left_recursion_start(ParseContext& pc) const noexcept {
            return parse(pc);
        }

        template <class ParseContext, class State>
        parse_result parse_left_recursion_continuation(ParseContext& pc, const State& match_start) const noexcept {
            return false;
        }

    private:
        Terminal m_terminal;
    };


    template <class Terminal>
    class terminal_sequence_parse_node : public parse_node<terminal_sequence_parse_node<Terminal>> {
    public:
        template <class It>
        terminal_sequence_parse_node(const It& begin, const It& end)
            : m_sequence(begin, end)
        {
        }

        template <class ParseContext>
        parse_result parse(ParseContext& pc) const noexcept {
            auto input_it = pc.parse_position();
            auto sequence_it = m_sequence.begin();
            for (;;) {
                if (sequence_it == m_sequence.end()) {
                    break;
                }
                if (input_it == pc.end_parse_position()) {
                    return false;
                }
                if (ParseContext::compare(*input_it, *sequence_it) != 0) {
                    return false;
                }
                ++input_it;
                ++sequence_it;
            }
            pc.set_parse_position(input_it);
            return true;
        }

        template <class ParseContext>
        parse_result parse_left_recursion_start(ParseContext& pc) const noexcept {
            return parse(pc);
        }

        template <class ParseContext, class State>
        parse_result parse_left_recursion_continuation(ParseContext& pc, const State& match_start) const noexcept {
            return false;
        }

    private:
        std::vector<Terminal> m_sequence;
    };


    template <class Terminal>
    class terminal_choice_parse_node : public parse_node<terminal_choice_parse_node<Terminal>> {
    public:
        template <class It>
        terminal_choice_parse_node(const It& begin, const It& end)
            : m_set(begin, end)
        {
            std::sort(m_set.begin(), m_set.end());
        }

        template <class ParseContext>
        parse_result parse(ParseContext& pc) const noexcept {
            if (pc.is_valid_parse_position()) {
                const auto token = *pc.parse_position();
                auto it = std::upper_bound(m_set.begin(), m_set.end(), token, [](const auto& a, const auto& b) { return ParseContext::compare(a, b) < 0; });
                if (it != m_set.begin() && ParseContext::compare(token, *std::prev(it)) == 0) {
                    pc.increment_parse_position();
                    return true;
                }
            }
            return false;
        }

        template <class ParseContext>
        parse_result parse_left_recursion_start(ParseContext& pc) const noexcept {
            return parse(pc);
        }

        template <class ParseContext, class State>
        parse_result parse_left_recursion_continuation(ParseContext& pc, const State& match_start) const noexcept {
            return false;
        }

    private:
        std::vector<Terminal> m_set;
    };


    template <class Terminal>
    auto terminal(const Terminal& terminal) noexcept {
        return get_parse_node_wrapper(terminal);
    }


    template <class... T>
    auto set(const T&... t) noexcept {
        return get_parse_node_wrapper(std::set({t...}));
    }


    template <class T>
    auto set(const T* str) noexcept {
        const T* str_end = str;
        for (; *str_end; ++str_end) {};
        return get_parse_node_wrapper(std::set(str, str_end));
    }


    template <class Terminal>
    class terminal_range_parse_node : public parse_node<terminal_range_parse_node<Terminal>> {
    public:
        terminal_range_parse_node(const Terminal& min, const Terminal& max) noexcept
            : m_min(min)
            , m_max(max)
        {
            assert(min <= max);
        }

        template <class ParseContext>
        parse_result parse(ParseContext& pc) const noexcept {
            if (pc.is_valid_parse_position()) {
                const auto token = *pc.parse_position();
                if (ParseContext::compare(token, m_min) >= 0 && ParseContext::compare(token, m_max) <= 0) {
                    pc.increment_parse_position();
                    return true;
                }
            }
            return false;
        }

        template <class ParseContext>
        parse_result parse_left_recursion_start(ParseContext& pc) const noexcept {
            return parse(pc);
        }

        template <class ParseContext, class State>
        parse_result parse_left_recursion_continuation(ParseContext& pc, const State& match_start) const noexcept {
            return false;
        }

    private:
        Terminal m_min;
        Terminal m_max;
    };


    template <class Terminal>
    terminal_range_parse_node<Terminal> range(const Terminal& min, const Terminal& max) noexcept {
        return { min, max };
    }


} //namespace parserlib


#endif //PARSERLIB_TERMINAL_PARSE_NODES_HPP
