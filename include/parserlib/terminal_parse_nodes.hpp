#ifndef PARSERLIB_TERMINAL_PARSE_NODES_HPP
#define PARSERLIB_TERMINAL_PARSE_NODES_HPP


#include <algorithm>
#include <cassert>
#include "parse_node.hpp"


namespace parserlib {


    /**
     * A terminal parse node.
     * @param Terminal type of terminal value.
     */
    template <class Terminal>
    class terminal_parse_node : public parse_node<terminal_parse_node<Terminal>> {
    public:
        /**
         * The constructor.
         * @param terminal the terminal value to parse.
         */
        terminal_parse_node(const Terminal& terminal)
            : m_terminal(terminal)
        {
        }

        /**
         * Returns true if the token at the current parse position matches the specified terminal value,
         * false otherwise.
         * @return the parse context.
         * @return true on success, false on failure.
         */
        template <class ParseContext>
        parse_result parse(ParseContext& pc) const noexcept {
            if (pc.is_valid_parse_position() && ParseContext::compare(*pc.parse_position(), m_terminal) == 0) {
                pc.increment_parse_position();
                return true;
            }
            return false;
        }

        /**
         * Same as parse(pc), but for the first step of left recursion parsing.
         * @return the parse context.
         * @return true on success, false on failure.
         */
        template <class ParseContext>
        parse_result parse_left_recursion_start(ParseContext& pc) const noexcept {
            return parse(pc);
        }

        /**
         * Same as parse(pc), but for the subsequent steps of left recursion parsing.
         * @param pc the parse context.
         * @param match_start start state of left recursion.
         * @return always false, because terminals are not supposed to parse at this left recursion state.
         */
        template <class ParseContext, class State>
        parse_result parse_left_recursion_continuation(ParseContext& pc, const State& match_start) const noexcept {
            return false;
        }

    private:
        Terminal m_terminal;
    };


    /**
     * Parses a sequence of terminals.
     * @param Terminal type of terminal value.
     */
    template <class Terminal>
    class terminal_sequence_parse_node : public parse_node<terminal_sequence_parse_node<Terminal>> {
    public:
        /**
         * The constructor.
         * @param begin start of the sequence.
         * @param end end of the sequence.
         */
        template <class It>
        terminal_sequence_parse_node(const It& begin, const It& end)
            : m_sequence(begin, end)
        {
            assert(begin < end);
        }

        /**
         * Parses the input against the sequence.
         * @param pc the parse context.
         * @return true on success, false on failure.
         */
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

        /**
         * Same as parse(pc), but for the first step of left recursion parsing.
         * @return the parse context.
         * @return true on success, false on failure.
         */
        template <class ParseContext>
        parse_result parse_left_recursion_start(ParseContext& pc) const noexcept {
            return parse(pc);
        }

        /**
         * Same as parse(pc), but for the subsequent steps of left recursion parsing.
         * @param pc the parse context.
         * @param match_start start state of left recursion.
         * @return always false, because terminals are not supposed to parse at this left recursion state.
         */
        template <class ParseContext, class State>
        parse_result parse_left_recursion_continuation(ParseContext& pc, const State& match_start) const noexcept {
            return false;
        }

    private:
        std::vector<Terminal> m_sequence;
    };


    /**
     * Parses a choice of a terminal from a set.
     * @param Terminal type of terminal value.
     */
    template <class Terminal>
    class terminal_choice_parse_node : public parse_node<terminal_choice_parse_node<Terminal>> {
    public:
        /**
         * The constructor.
         * @param begin start of the range of terminals to create a choice out of.
         * @param end end of the range of terminals to create a choice out of.
         */
        template <class It>
        terminal_choice_parse_node(const It& begin, const It& end)
            : m_set(begin, end)
        {
            std::sort(m_set.begin(), m_set.end());
        }

        /**
         * Parses a token against a set of terminals.
         * The set of terminals is a sorted std::vector, and std::upper_bound is used to search,
         *  thus providing very good performance.
         * @param pc the parse context.
         * @return true if the current token is within the set, false otherwise.
         */
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

        /**
         * Same as parse(pc), but for the first step of left recursion parsing.
         * @return the parse context.
         * @return true on success, false on failure.
         */
        template <class ParseContext>
        parse_result parse_left_recursion_start(ParseContext& pc) const noexcept {
            return parse(pc);
        }

        /**
         * Same as parse(pc), but for the subsequent steps of left recursion parsing.
         * @param pc the parse context.
         * @param match_start start state of left recursion.
         * @return always false, because terminals are not supposed to parse at this left recursion state.
         */
        template <class ParseContext, class State>
        parse_result parse_left_recursion_continuation(ParseContext& pc, const State& match_start) const noexcept {
            return false;
        }

    private:
        std::vector<Terminal> m_set;
    };


    /**
     * Helper function for creating a terminal parse node.
     * @param terminal terminal value to create a terminal parse node for.
     * @return a parse node for the given terminal.
     */
    template <class Terminal>
    auto terminal(const Terminal& terminal) noexcept {
        return get_parse_node_wrapper(terminal);
    }


    /**
     * Helper function for creating a terminal choice parser out of a set of terminals.
     * @param t terminals to create the terminal choice parser of.
     * @return a terminal choice parser.
     */
    template <class... T>
    auto set(const T&... t) noexcept {
        return get_parse_node_wrapper(std::set({t...}));
    }


    /**
     * Helper function for creating a terminal choice parser out of a null-terminated string.
     * @param t terminals to create the terminal choice parser of.
     * @return a terminal choice parser.
     */
    template <class T>
    auto set(const T* str) noexcept {
        const T* str_end = str;
        for (; *str_end; ++str_end) {};
        return get_parse_node_wrapper(std::set(str, str_end));
    }


    /**
     * A terminal parse node that parses a token against a range of terminals (min to max).
     * @param Terminal type of terminal.
     */
    template <class Terminal>
    class terminal_range_parse_node : public parse_node<terminal_range_parse_node<Terminal>> {
    public:
        /**
         * The constructor.
         * @param min the min value of the range.
         * @param max the max value of the range.
         */
        terminal_range_parse_node(const Terminal& min, const Terminal& max) noexcept
            : m_min(min)
            , m_max(max)
        {
            assert(min <= max);
        }

        /**
         * If the current token false within the min-max range, then it returns true,
         * otherwise it returns false.
         * @param pc the parse contedxt.
         * @return true on success, false on failure.
         */
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

        /**
         * Same as parse(pc), but for the first step of left recursion parsing.
         * @return the parse context.
         * @return true on success, false on failure.
         */
        template <class ParseContext>
        parse_result parse_left_recursion_start(ParseContext& pc) const noexcept {
            return parse(pc);
        }

        /**
         * Same as parse(pc), but for the subsequent steps of left recursion parsing.
         * @param pc the parse context.
         * @param match_start start state of left recursion.
         * @return always false, because terminals are not supposed to parse at this left recursion state.
         */
        template <class ParseContext, class State>
        parse_result parse_left_recursion_continuation(ParseContext& pc, const State& match_start) const noexcept {
            return false;
        }

    private:
        Terminal m_min;
        Terminal m_max;
    };


    /**
     * Helper function for creating a range.
     * @param min the min value of the range.
     * @param max the max value of the range.
     * @return a terminal range.
     */
    template <class Terminal>
    terminal_range_parse_node<Terminal> range(const Terminal& min, const Terminal& max) noexcept {
        return { min, max };
    }


} //namespace parserlib


#endif //PARSERLIB_TERMINAL_PARSE_NODES_HPP
