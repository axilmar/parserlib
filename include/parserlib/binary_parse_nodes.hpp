#ifndef PARSERLIB_BINARY_PARSE_NODES_HPP
#define PARSERLIB_BINARY_PARSE_NODES_HPP


#include <tuple>
#include <utility>
#include "parse_node.hpp"


namespace parserlib {


    class sequence_parse_node_base {};


    /**
     * A parse node that parses other parse nodes in sequence.
     * All parse nodes must return true in order for this node to return true.
     * @param Parsers parse nodes to invoke.
     */
    template <class... Parsers>
    class sequence_parse_node : public parse_node<sequence_parse_node<Parsers...>>, public sequence_parse_node_base {
    public:
        /** Tuple of parsers. */
        using tuple_type = std::tuple<Parsers...>;

        /**
         * The constructor.
         * @param parsers tuple of parsers.
         */
        sequence_parse_node(const tuple_type& parsers) noexcept
            : m_parsers(parsers)
        {
        }

        /**
         * Invokes the specified parsers in sequence.
         * If a parser returns false, then parsing is stopped and the parse context is rewinded 
         * to the state that it was before calling this function.
         * @param pc the parse context.
         * @return true if parsing succeeded, false if it failed, left recursion if left recursion is detected.
         */
        template <class ParseContext>
        parse_result parse(ParseContext& pc) const noexcept {
            return parse_loop<0>(pc.state(), pc);
        }

        /**
         * Same as parse(pc), but for the first step of left recursion parsing.
         * @param pc the parse context.
         * @return true if parsing succeeded, false if it failed, left recursion if left recursion is detected.
         */
        template <class ParseContext>
        parse_result parse_left_recursion_start(ParseContext& pc) const noexcept {
            return parse_left_recursion_start_loop(pc.state(), pc);
        }

        /**
         * Same as parse(pc), but for the subsequent steps of left recursion parsing.
         * @param pc the parse context.
         * @param match_start start state of left recursion.
         * @return true if parsing succeeded, false if it failed, left recursion if left recursion is detected.
         */
        template <class ParseContext, class State>
        parse_result parse_left_recursion_continuation(ParseContext& pc, const State& match_start) const noexcept {
            return parse_left_recursion_continuation_loop(pc.state(), pc, match_start);
        }

        /**
         * Returns the parsers of the sequence.
         * @return the parsers of the sequence.
         */
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


    /**
     * An operator that returns a sequence out of two values; one of the values must be a parse node.
     * @param l the left value; promoted to the appropriate parse node type automatically, if not a parse node.
     * @param r the right value; promoted to the appropriate parse node type automatically, if not a parse node.
     * @return a sequence of parse nodes.
     */
    template <class L, class R, std::enable_if_t<(std::is_base_of_v<parse_node_base, std::decay_t<L>> || std::is_base_of_v<parse_node_base, std::decay_t<R>>) && !std::is_base_of_v<sequence_parse_node_base, L> && !std::is_base_of_v<sequence_parse_node_base, R>, bool> = true>
    auto operator >> (L&& l, R&& r) noexcept {
        return sequence_parse_node(std::make_tuple(get_parse_node_wrapper(std::forward<L>(l)), get_parse_node_wrapper(std::forward<R>(r))));
    }


    /**
     * An operator that merges a sequence of parse nodes with a value.
     * @param l the sequence.
     * @param r the value; promoted to the appropriate parse node type automatically, if not a parse node.
     * @return a sequence of parse nodes that includes the parse nodes of the sequence and the parse node for the value.
     */
    template <class... L, class... R>
    auto operator >> (const sequence_parse_node<L...>& l, const sequence_parse_node<R...>& r) noexcept {
        return sequence_parse_node(std::tuple_cat(l.parsers(), r.parsers()));
    }


    /**
     * An operator that merges a value with a sequence of parse nodes.
     * @return l the value; promoted to the appropriate parse node type automatically, if not a parse node.
     * @param l the sequence.
     * @return a sequence of parse nodes that includes the parse node for the value and the parse nodes of the sequence.
     */
    template <class... L, class R, std::enable_if_t<!std::is_base_of_v<sequence_parse_node_base, R>, bool> = true>
    auto operator >> (const sequence_parse_node<L...>& l, R&& r) noexcept {
        return sequence_parse_node(std::tuple_cat(l.parsers(), std::make_tuple(get_parse_node_wrapper(std::forward<R>(r)))));
    }


    /**
     * An operator that merges two sequences.
     * @param l the left sequence.
     * @param r the right sequence.
     * @return a sequence of nodes that includes the nodes of the left and the right sequences.
     */
    template <class L, class... R, std::enable_if_t<!std::is_base_of_v<sequence_parse_node_base, L>, bool> = true>
    auto operator >> (L&& l, const sequence_parse_node<R...>& r) noexcept {
        return sequence_parse_node(std::tuple_cat(std::make_tuple(get_parse_node_wrapper(std::forward<L>(l))), r.parsers()));
    }


    class choice_parse_node_base {};


    /**
     * A parse node that parses other parse nodes one at a time.
     * The parsing stops if one of the parsers returns true.
     * @param Parsers parse nodes to invoke.
     */
    template <class... Parsers>
    class choice_parse_node : public parse_node<choice_parse_node<Parsers...>>, public choice_parse_node_base {
    public:
        /** Tuple of parsers. */
        using tuple_type = std::tuple<Parsers...>;

        /**
         * The constructor.
         * @param parsers tuple of parsers.
         */
        choice_parse_node(const tuple_type& parsers) noexcept
            : m_parsers(parsers)
        {
        }

        /**
         * Invokes the specified parsers, one at a time.
         * If a parser returns true, then parsing ends and true is returned.
         * @param pc the parse context.
         * @return true if parsing succeeded, false if it failed, left recursion if left recursion is detected.
         */
        template <class ParseContext>
        parse_result parse(ParseContext& pc) const noexcept {
            return parse_loop<0>(pc.state(), pc);
        }

        /**
         * Same as parse(pc), but for the first step of left recursion parsing.
         * @param pc the parse context.
         * @return true if parsing succeeded, false if it failed, left recursion if left recursion is detected.
         */
        template <class ParseContext>
        parse_result parse_left_recursion_start(ParseContext& pc) const noexcept {
            return parse_left_recursion_start_loop<0>(pc.state(), pc);
        }

        /**
         * Same as parse(pc), but for the subsequent steps of left recursion parsing.
         * @param pc the parse context.
         * @param match_start start state of left recursion.
         * @return true if parsing succeeded, false if it failed, left recursion if left recursion is detected.
         */
        template <class ParseContext, class State>
        parse_result parse_left_recursion_continuation(ParseContext& pc, const State& match_start) const noexcept {
            return parse_left_recursion_continuation_loop<0>(pc.state(), pc, match_start);
        }

        /**
         * Returns the parsers of the choice.
         * @return the parsers of the choice.
         */
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
                    case parse_result::FALSE:
                        break;
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
                    case parse_result::FALSE:
                        break;
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
                    case parse_result::FALSE:
                        break;
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


    /**
     * An operator that returns a choice out of two values; one of the values must be a parse node.
     * @param l the left value; promoted to the appropriate parse node type automatically, if not a parse node.
     * @param r the right value; promoted to the appropriate parse node type automatically, if not a parse node.
     * @return a choice of parse nodes.
     */
    template <class L, class R, std::enable_if_t<(std::is_base_of_v<parse_node_base, std::decay_t<L>> || std::is_base_of_v<parse_node_base, std::decay_t<R>>) && !std::is_base_of_v<choice_parse_node_base, L> && !std::is_base_of_v<choice_parse_node_base, R>, bool> = true>
    auto operator | (L&& l, R&& r) noexcept {
        return choice_parse_node(std::make_tuple(get_parse_node_wrapper(std::forward<L>(l)), get_parse_node_wrapper(std::forward<R>(r))));
    }


    /**
     * An operator that merges a choice of parse nodes with a value.
     * @param l the choice.
     * @param r the value; promoted to the appropriate parse node type automatically, if not a parse node.
     * @return a choice of parse nodes that includes the parse nodes of the choice and the parse node for the value.
     */
    template <class... L, class... R>
    auto operator | (const choice_parse_node<L...>& l, const choice_parse_node<R...>& r) noexcept {
        return choice_parse_node(std::tuple_cat(l.parsers(), r.parsers()));
    }


    /**
     * An operator that merges a value with a choice of parse nodes.
     * @return l the value; promoted to the appropriate parse node type automatically, if not a parse node.
     * @param l the choice.
     * @return a choice of parse nodes that includes the parse node for the value and the parse nodes of the choice.
     */
    template <class... L, class R, std::enable_if_t<!std::is_base_of_v<choice_parse_node_base, R>, bool> = true>
    auto operator | (const choice_parse_node<L...>& l, R&& r) noexcept {
        return choice_parse_node(std::tuple_cat(l.parsers(), std::make_tuple(get_parse_node_wrapper(std::forward<R>(r)))));
    }


    /**
     * An operator that merges two choices.
     * @param l the left choice.
     * @param r the right choice.
     * @return a choice of nodes that includes the nodes of the left and the right choices.
     */
    template <class L, class... R, std::enable_if_t<!std::is_base_of_v<choice_parse_node_base, L>, bool> = true>
    auto operator | (L&& l, const choice_parse_node<R...>& r) noexcept {
        return choice_parse_node(std::tuple_cat(std::make_tuple(get_parse_node_wrapper(std::forward<L>(l))), r.parsers()));
    }


    /**
     * An operator that turns the operation 'l - r' to the sequence '!r >> l'.
     * It allows an expression 'l' to parse to true unless the current token sequence can be parsed with parser 'r'.
     * @param l the left value; promoted to the appropriate parse node type automatically, if not a parse node.
     * @param r the right value; promoted to the appropriate parse node type automatically, if not a parse node.
     * @return the sequence '!r >> l'.
     */
    template <class L, class R, std::enable_if_t<std::is_base_of_v<parse_node_base, std::decay_t<L>> || std::is_base_of_v<parse_node_base, std::decay_t<R>>, bool> = true>
    auto operator - (L&& l, R&& r) noexcept {
        return !get_parse_node_wrapper(std::forward<R>(r)) >> get_parse_node_wrapper(std::forward<L>(l));
    }


    /**
     * A parse node that adds a match to the parse context when the given parse node parses successfully.
     * @param Parser the parser to use for parsing.
     * @param MatchId id of the match to put to the parse context.
     */
    template <class Parser, class MatchId>
    class match_parser_node : public parse_node<match_parser_node<Parser, MatchId>> {
    public:
        /**
         * The constructor.
         * @param parser the parser to use for parsing.
         * @param match_id id of the match to put to the parse context.
         */
        match_parser_node(const Parser& parser, const MatchId& match_id) noexcept
            : m_parser(parser)
            , m_match_id(match_id)
        {
        }

        /**
         * Invokes the specified parser, and if it returns true, then adds a match to the parse context.
         * @param pc the parse context.
         * @return true if parsing succeeds, false if it failed, left recursion if left recursion is detected.
         */
        template <class ParseContext>
        parse_result parse(ParseContext& pc) const noexcept {
            const auto match_start = pc.state();
            parse_result result = m_parser.parse(pc);
            if (result) {
                pc.add_match(m_match_id, match_start.position(), pc.parse_position(), match_start.match_count());
            }
            return result;
        }

        /**
         * Same as parse(pc), but for the first step of left recursion parsing.
         * @param pc the parse context.
         * @return true if parsing succeeded, false if it failed, left recursion if left recursion is detected.
         */
        template <class ParseContext>
        parse_result parse_left_recursion_start(ParseContext& pc) const noexcept {
            const auto match_start = pc.state();
            parse_result result = m_parser.parse_left_recursion_start(pc);
            if (result) {
                pc.add_match(m_match_id, match_start.position(), pc.parse_position(), match_start.match_count());
            }
            return result;
        }

        /**
         * Same as parse(pc), but for the subsequent steps of left recursion parsing.
         * @param pc the parse context.
         * @param match_start start state of left recursion.
         * @return true if parsing succeeded, false if it failed, left recursion if left recursion is detected.
         */
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


    /**
     * An operator that creates a match parser.
     * @param parser the parser to use for parsing.
     * @param match_id id of the match to put to the parse context.
     * @return the match parser.
     */
    template <class Parser, class MatchId>
    auto operator ->* (Parser&& parser, const MatchId& match_id) noexcept {
        return match_parser_node(get_parse_node_wrapper(std::forward<Parser>(parser)), match_id);
    }


} //namespace parserlib


#endif //PARSERLIB_BINARY_PARSE_NODES_HPP
