#ifndef PARSERLIB_ERROR_HANDLING_PARSE_NODES_HPP
#define PARSERLIB_ERROR_HANDLING_PARSE_NODES_HPP


#include <algorithm>
#include "parse_node.hpp"


namespace parserlib {


    // ERROR SKIP POLICIES ----------------------------------------------------


    /**
     * Base class for skip policies.
     * Used to recognize if a parse node class is a skip policy.
     */
    class skip_policy_base {};


    /**
     * A skip policy that does nothing.
     */
    class skip_nothing_parse_node : public parse_node<skip_nothing_parse_node>, public skip_policy_base {
    public:
        /**
         * The parse function.
         * It does nothing.
         * @param pc the parse context.
         * @return always true.
         */
        template <class ParseContext>
        parse_result parse(ParseContext& pc) const noexcept {
            return true;
        }

        /**
         * Same as parse(pc), but for the first step of left recursion parsing.
         * @param pc the parse context.
         * @return always true.
         */
        template <class ParseContext>
        parse_result parse_left_recursion_start(ParseContext& pc) const noexcept {
            return true;
        }

        /**
         * Same as parse(pc), but for the subsequent steps of left recursion parsing.
         * @param pc the parse context.
         * @param match_start start state of left recursion.
         * @return true if parsing succeeded, false if it failed, left recursion if left recursion is detected.
         */
        template <class ParseContext, class State>
        parse_result parse_left_recursion_continuation(ParseContext& pc, const State& match_start) const noexcept {
            return true;
        }
    };


    /**
     * Helper function for creating a skip nothing parse node.
     * @return a skip nothing parse node.
     */
    inline skip_nothing_parse_node skip_nothing() noexcept {
        return {};
    }


    /**
     * A class that skips input until a specific grammar parses successfully.
     * Upon successful parsing, the parse context position is set to the first token
     * that the specified parser recognized successfully.
     * @param Parser parser to use for recognizing input.
     */
    template <class Parser>
    class skip_until_parse_node : public parse_node<skip_until_parse_node<Parser>>, public skip_policy_base {
    public:
        /**
         * The constructor.
         * @param parser the parser to use for recognizing the input.
         */
        skip_until_parse_node(const Parser& parser) noexcept
            : m_parser(parser)
        {
        }

        /**
         * Parses the input until the specified parser returns true.
         * On exit, the state of the parse context is set to the first token
         * that the specified parser recognized.
         * Side effects like adding matches are ignored.
         * @param pc the parse context.
         * @return always true.
         */
        template <class ParseContext>
        parse_result parse(ParseContext& pc) const noexcept {
            const auto initial_state = pc.state();
            while (pc.is_valid_parse_position()) {
                const auto parse_position = pc.parse_position();
                if (!m_parser.parse(pc)) {
                    pc.increment_parse_position();
                }
                else {
                    pc.set_state(initial_state);
                    pc.set_parse_position(parse_position);
                    return true;
                }
            }
            return true;
        }

        /**
         * Same as parse(pc), but for the first step of left recursion parsing.
         * @param pc the parse context.
         * @return always true.
         */
        template <class ParseContext>
        parse_result parse_left_recursion_start(ParseContext& pc) const noexcept {
            const auto initial_state = pc.state();
            while (pc.is_valid_parse_position()) {
                const auto parse_position = pc.parse_position();
                if (!m_parser.parse_left_recursion_start(pc)) {
                    pc.increment_parse_position();
                }
                else {
                    pc.set_state(initial_state);
                    pc.set_parse_position(parse_position);
                    return true;
                }
            }
            return true;
        }

        /**
         * Same as parse(pc), but for the subsequent steps of left recursion parsing.
         * @param pc the parse context.
         * @param match_start start state of left recursion.
         * @return true if parsing succeeded, false if it failed, left recursion if left recursion is detected.
         */
        template <class ParseContext, class State>
        parse_result parse_left_recursion_continuation(ParseContext& pc, const State& match_start) const noexcept {
            const auto initial_state = pc.state();
            while (pc.is_valid_parse_position()) {
                const auto parse_position = pc.parse_position();
                if (!m_parser.parse_left_recursion_continuation(pc, match_start)) {
                    pc.increment_parse_position();
                }
                else {
                    pc.set_state(initial_state);
                    pc.set_parse_position(parse_position);
                    return true;
                }
            }
            return true;
        }

    private:
        Parser m_parser;
    };


    /**
     * A helper function that creates a skip until parse node.
     * @param parser the parser to use for recognizing the input.
     * A skip until parse node.
     */
    template <class Parser>
    auto skip_until(Parser&& parser) {
        return skip_until_parse_node(get_parse_node_wrapper(std::forward<Parser>(parser)));
    }


    /**
     * A class that skips input until a specific grammar parses successfully.
     * Upon successful parsing, the parse context position is set to the first token
     * after the token sequence that the specified parser recognized successfully.
     * @param Parser parser to use for recognizing input.
     */
    template <class Parser>
    class skip_until_after_parse_node : public parse_node<skip_until_after_parse_node<Parser>>, public skip_policy_base {
    public:
        /**
         * The constructor.
         * @param parser the parser to use for recognizing the input.
         */
        skip_until_after_parse_node(const Parser& parser) noexcept
            : m_parser(parser)
        {
        }

        /**
         * Parses the input until the specified parser returns true.
         * On exit, the state of the parse context is set to the first token
         * after the token sequence that the specified parser recognized.
         * Side effects like adding matches are ignored.
         * @param pc the parse context.
         * @return always true.
         */
        template <class ParseContext>
        parse_result parse(ParseContext& pc) const noexcept {
            const auto initial_state = pc.state();
            while (pc.is_valid_parse_position()) {
                if (!m_parser.parse(pc)) {
                    pc.increment_parse_position();
                }
                else {
                    const auto parse_position = pc.parse_position();
                    pc.set_state(initial_state);
                    pc.set_parse_position(parse_position);
                    return true;
                }
            }
            return true;
        }

        /**
         * Same as parse(pc), but for the first step of left recursion parsing.
         * @param pc the parse context.
         * @return always true.
         */
        template <class ParseContext>
        parse_result parse_left_recursion_start(ParseContext& pc) const noexcept {
            const auto initial_state = pc.state();
            while (pc.is_valid_parse_position()) {
                if (!m_parser.parse_left_recursion_start(pc)) {
                    pc.increment_parse_position();
                }
                else {
                    const auto parse_position = pc.parse_position();
                    pc.set_state(initial_state);
                    pc.set_parse_position(parse_position);
                    return true;
                }
            }
            return true;
        }

        /**
         * Same as parse(pc), but for the subsequent steps of left recursion parsing.
         * @param pc the parse context.
         * @param match_start start state of left recursion.
         * @return true if parsing succeeded, false if it failed, left recursion if left recursion is detected.
         */
        template <class ParseContext, class State>
        parse_result parse_left_recursion_continuation(ParseContext& pc, const State& match_start) const noexcept {
            const auto initial_state = pc.state();
            while (pc.is_valid_parse_position()) {
                if (!m_parser.parse_left_recursion_continuation(pc, match_start)) {
                    pc.increment_parse_position();
                }
                else {
                    const auto parse_position = pc.parse_position();
                    pc.set_state(initial_state);
                    pc.set_parse_position(parse_position);
                    return true;
                }
            }
            return true;
        }

    private:
        Parser m_parser;
    };


    /**
     * A helper function that creates a skip until after parse node.
     * @param parser the parser to use for recognizing the input.
     * A skip until after parse node.
     */
    template <class Parser>
    auto skip_until_after(Parser&& parser) {
        return skip_until_after_parse_node(get_parse_node_wrapper(std::forward<Parser>(parser)));
    }


    /**
     * A class that skips input while a specific grammar parses successfully.
     * Upon successful parsing, the parse context position is set to the first token
     * that the specified parser did not recognize.
     * @param Parser parser to use for recognizing input.
     */
    template <class Parser>
    class skip_while_parse_node : public parse_node<skip_while_parse_node<Parser>>, public skip_policy_base {
    public:
        /**
         * The constructor.
         * @param parser the parser to use for recognizing the input.
         */
        skip_while_parse_node(const Parser& parser) noexcept
            : m_parser(parser)
        {
        }

        /**
         * Parses the input until the specified parser returns true.
         * On exit, the parse context state is set to the first token
         * that the specified parser did not recognize.
         * Side effects like adding matches are ignored.
         * @param pc the parse context.
         * @return always true.
         */
        template <class ParseContext>
        parse_result parse(ParseContext& pc) const noexcept {
            const auto initial_state = pc.state();
            while (pc.is_valid_parse_position()) {
                const auto parse_position = pc.parse_position();
                if (m_parser.parse(pc)) {
                    pc.increment_parse_position();
                }
                else {
                    pc.set_state(initial_state);
                    pc.set_parse_position(parse_position);
                    return true;
                }
            }
            return true;
        }

        /**
         * Same as parse(pc), but for the first step of left recursion parsing.
         * @param pc the parse context.
         * @return always true.
         */
        template <class ParseContext>
        parse_result parse_left_recursion_start(ParseContext& pc) const noexcept {
            const auto initial_state = pc.state();
            while (pc.is_valid_parse_position()) {
                const auto parse_position = pc.parse_position();
                if (m_parser.parse_left_recursion_start(pc)) {
                    pc.increment_parse_position();
                }
                else {
                    pc.set_state(initial_state);
                    pc.set_parse_position(parse_position);
                    return true;
                }
            }
            return true;
        }

        /**
         * Same as parse(pc), but for the subsequent steps of left recursion parsing.
         * @param pc the parse context.
         * @param match_start start state of left recursion.
         * @return true if parsing succeeded, false if it failed, left recursion if left recursion is detected.
         */
        template <class ParseContext, class State>
        parse_result parse_left_recursion_continuation(ParseContext& pc, const State& match_start) const noexcept {
            const auto initial_state = pc.state();
            while (pc.is_valid_parse_position()) {
                const auto parse_position = pc.parse_position();
                if (m_parser.parse_left_recursion_continuation(pc, match_start)) {
                    pc.increment_parse_position();
                }
                else {
                    pc.set_state(initial_state);
                    pc.set_parse_position(parse_position);
                    return true;
                }
            }
            return true;
        }

    private:
        Parser m_parser;
    };


    /**
     * A helper function that creates a skip while parse node.
     * @param parser the parser to use for recognizing the input.
     * A skip while parse node.
     */
    template <class Parser>
    auto skip_while(Parser&& parser) {
        return skip_while_parse_node(get_parse_node_wrapper(std::forward<Parser>(parser)));
    }


    /**
     * A parse node that skips a specific amount of input tokens.
     */
    class skip_count_parse_node : public parse_node<skip_count_parse_node>, public skip_policy_base {
    public:
        /**
         * The constructor.
         * @param count number of tokens to skip.
         */
        skip_count_parse_node(std::size_t count) noexcept
            : m_count(count)
        {
        }

        /**
         * Skips the specified amount of tokens until the end of input.
         * @param pc the parser context.
         * @return always true.
         */
        template <class ParseContext>
        parse_result parse(ParseContext& pc) const noexcept {
            for (std::size_t count = m_count; pc.is_valid_parse_position() && count > 0; --count) {
                pc.increment_parse_position();
            }
            return true;
        }

        /**
         * Same as parse(pc), but for the first step of left recursion parsing.
         * @param pc the parse context.
         * @return always true.
         */
        template <class ParseContext>
        parse_result parse_left_recursion_start(ParseContext& pc) const noexcept {
            return parse(pc);
        }

        /**
         * Same as parse(pc), but for the subsequent steps of left recursion parsing.
         * @param pc the parse context.
         * @param match_start start state of left recursion.
         * @return true if parsing succeeded, false if it failed, left recursion if left recursion is detected.
         */
        template <class ParseContext, class State>
        parse_result parse_left_recursion_continuation(ParseContext& pc, const State& match_start) const noexcept {
            return parse(pc);
        }

    private:
        std::size_t m_count;
    };


    /**
     * Helper function for creating a skip count parse node.
     * @param count number of tokens to skip.
     * @return a skip count parse node.
     */
    inline skip_count_parse_node skip_count(std::size_t count) {
        return count;
    }


    /**
     * Returns a skip count parse node with count = 1.
     * @return a skip count parse node with count = 1.
     */
    inline skip_count_parse_node skip_current() {
        return 1;
    }


    // ERROR HANDLER ----------------------------------------------------------


    /**
     * An error handler parse node.
     * @param ErrorId id of error to put to the parse context.
     * @param SkipPolicy the skip policy.
     */
    template <class ErrorId, class SkipPolicy>
    class error_handler_parse_node : public parse_node<error_handler_parse_node<ErrorId, SkipPolicy>> {
    public:
        /** The error id type. */
        using error_id_type = ErrorId;

        /** The skip policy type. */
        using skip_policy_type = SkipPolicy;

        /**
         * The constructor.
         * @param error_id id of error to put to the parse context.
         * @param skip_policy the skip policy.
         */
        error_handler_parse_node(const error_id_type& error_id, const skip_policy_type& skip_policy) noexcept
            : m_error_id(error_id)
            , m_skip_policy(skip_policy)
        {
        }

        /**
         * Invokes the skip policy to advance the parser context's iterator,
         * then places an error in the parse context with the specified error id.
         * The range of error is from the parse position when this function is entered
         * up to the parse position after the skip policy finishes parsing.
         * @param pc the parse context.
         * @return always true.
         */
        template <class ParseContext>
        parse_result parse(ParseContext& pc) const noexcept {
            if (pc.is_valid_parse_position()) {
                const auto error_start = pc.parse_position();
                m_skip_policy.parse(pc);
                const auto error_end = pc.parse_position();
                pc.add_error(m_error_id, error_start, error_end == error_start ? std::next(error_end) : error_end);
                return true;
            }
            return false;
        }

        /**
         * Same as parse(pc), but for the first step of left recursion parsing.
         * @param pc the parse context.
         * @return always true.
         */
        template <class ParseContext>
        parse_result parse_left_recursion_start(ParseContext& pc) const noexcept {
            return parse(pc);
        }

        /**
         * Same as parse(pc), but for the subsequent steps of left recursion parsing.
         * @param pc the parse context.
         * @param match_start start state of left recursion.
         * @return true if parsing succeeded, false if it failed, left recursion if left recursion is detected.
         */
        template <class ParseContext, class State>
        parse_result parse_left_recursion_continuation(ParseContext& pc, const State& match_start) const noexcept {
            return false;
        }

    private:
        error_id_type m_error_id;
        skip_policy_type m_skip_policy;
    };


    /**
     * Helper function for creating an error parse node with a skip nothing policy.
     * @param error_id id of error to put to the parse context.
     * @return an error parse node with a skip nothing policy.
     */
    template <class ErrorId>
    auto error(const ErrorId& error_id) noexcept {
        return error_handler_parse_node(error_id, skip_nothing());
    }


    /**
     * Helper function for creating an error parse node with the given skip policy.
     * @param error_id id of error to put to the parse context.
     * @param skip_policy the skip policy.
     * @return an error parse node with the given skip policy.
     */
    template <class ErrorId, class SkipPolicy>
    auto error(const ErrorId& error_id, const SkipPolicy& skip_policy) noexcept {
        return error_handler_parse_node(error_id, skip_policy);
    }


    // ERROR MATCH ------------------------------------------------------------


    /**
     * A match parse node that places a match into the parse context when there is an error.
     * It allows the replacement of missing tokens with an error parse node.
     * @param MatchId id of the match.
     * @param SkipPolicy the skip policy.
     */
    template <class MatchId, class SkipPolicy>
    class error_match_parse_node : public parse_node<error_match_parse_node<MatchId, SkipPolicy>> {
    public:
        /** The match id type. */
        using match_id_type = MatchId;

        /** The skip policy type. */
        using skip_policy_type = SkipPolicy;

        /**
         * The constructor.
         * @param match_id id of the match.
         * @param skip_policy the skip policy.
         */
        error_match_parse_node(const match_id_type& match_id, const skip_policy_type& skip_policy) noexcept
            : m_match_id(match_id)
            , m_skip_policy(skip_policy)
        {
        }

        /**
         * Invokes the skip policy to advance the parser context's iterator,
         * then places a match in the parse context with the specified match id.
         * The range of match is from the parse position when this function is entered
         * up to the parse position after the skip policy finishes parsing.
         * @param pc the parse context.
         * @return always true.
         */
        template <class ParseContext>
        parse_result parse(ParseContext& pc) const noexcept {
            if (pc.is_valid_parse_position()) {
                const auto start = pc.parse_position();
                m_skip_policy.parse(pc);
                const auto end = pc.parse_position();
                pc.add_match(m_match_id, start, end == start ? std::next(end) : end);
                return true;
            }
            return false;
        }

        /**
         * Same as parse(pc), but for the first step of left recursion parsing.
         * @param pc the parse context.
         * @return always true.
         */
        template <class ParseContext>
        parse_result parse_left_recursion_start(ParseContext& pc) const noexcept {
            return parse(pc);
        }

        /**
         * Same as parse(pc), but for the subsequent steps of left recursion parsing.
         * @param pc the parse context.
         * @param match_start start state of left recursion.
         * @return true if parsing succeeded, false if it failed, left recursion if left recursion is detected.
         */
        template <class ParseContext, class State>
        parse_result parse_left_recursion_continuation(ParseContext& pc, const State& match_start) const noexcept {
            return false;
        }

    private:
        MatchId m_match_id;
        SkipPolicy m_skip_policy;
    };


    /**
     * Helper function for creating an error match parse node with a skip nothing policy.
     * @param match_id id of match to put to the parse context.
     * @return an error match parse node with a skip nothing policy.
     */
    template <class MatchId>
    auto error_match(const MatchId& match_id) noexcept {
        return error_match_parse_node(match_id, skip_nothing());
    }


    /**
     * Helper function for creating an error match parse node with the given skip policy.
     * @param match_id id of match to put to the parse context.
     * @param skip_policy the skip policy.
     * @return an error match parse node with the given skip policy.
     */
    template <class MatchId, class SkipPolicy>
    auto error_match(const MatchId& match_id, const SkipPolicy& skip_policy) noexcept {
        return error_match_parse_node(match_id, skip_policy);
    }


} //namespace parserlib


#endif //PARSERLIB_ERROR_HANDLING_PARSE_NODES_HPP
