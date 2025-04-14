#ifndef PARSERLIB_UNARY_PARSE_NODES_HPP
#define PARSERLIB_UNARY_PARSE_NODES_HPP


#include "parse_node.hpp"


namespace parserlib {


    template <class Parser, class ParseContext>
    parse_result do_optional_parse_loop(const Parser& parser, ParseContext& pc) noexcept {
        while (pc.is_valid_parse_position()) {
            const auto start = pc.parse_position();
            parse_result result = parser.parse(pc);
            switch (result.value()) {
            case parse_result::FALSE:
                return true;

            case parse_result::TRUE:
                if (pc.parse_position() == start) {
                    return true;
                }
                continue;

            case parse_result::LEFT_RECURSION:
                return result;
            }
        }
        return true;
    }


    /**
     * A parse node that repeats parsing using another parse node zero or more times.
     * @param Parser the parse node to repeat.
     */
    template <class Parser>
    class zero_or_more_parse_node : public parse_node<zero_or_more_parse_node<Parser>> {
    public:
        /**
         * The constructor.
         * @param parser the parse node to use in repetition.
         */
        zero_or_more_parse_node(const Parser& parser) noexcept
            : m_parser(parser)
        {
        }

        /**
         * Repeats the parsing zero or more times.
         * @param pc the parse context.
         * @return either true or left recursion.
         */
        template <class ParseContext>
        parse_result parse(ParseContext& pc) const noexcept {
            return do_optional_parse_loop(m_parser, pc);
        }

        /**
         * Same as parse(pc), but for the first step of left recursion parsing.
         * @param pc the parse context.
         * @return either true or left recursion.
         */
        template <class ParseContext>
        parse_result parse_left_recursion_start(ParseContext& pc) const noexcept {
            parse_result result = m_parser.parse_left_recursion_start(pc);
            switch (result.value()) {
                case parse_result::FALSE:
                    return true;
                case parse_result::TRUE:
                    break;
                case parse_result::LEFT_RECURSION:
                    return result;
            }
            return do_optional_parse_loop(m_parser, pc);
        }

        /**
         * Same as parse(pc), but for the subsequent steps of left recursion parsing.
         * @param pc the parse context.
         * @param match_start start state of left recursion.
         * @return either true or left recursion.
         */
        template <class ParseContext, class State>
        parse_result parse_left_recursion_continuation(ParseContext& pc, const State& match_start) const noexcept {
            parse_result result = m_parser.parse_left_recursion_continuation(pc, match_start);
            switch (result.value()) {
                case parse_result::FALSE:
                    return true;
                case parse_result::TRUE:
                    break;
                case parse_result::LEFT_RECURSION:
                    return result;
            }
            return do_optional_parse_loop(m_parser, pc);
        }

    private:
        Parser m_parser;
    };


    /**
     * A parse node that repeats parsing using another parse node one or more times.
     * @param Parser the parse node to repeat.
     */
    template <class Parser>
    class one_or_more_parse_node : public parse_node<one_or_more_parse_node<Parser>> {
    public:
        /**
         * The constructor.
         * @param parser the parse node to use in repetition.
         */
        one_or_more_parse_node(const Parser& parser) noexcept
            : m_parser(parser)
        {
        }

        /**
         * Repeats the parsing one or more times.
         * @param pc the parse context.
         * @return false if it fails the first time, otherwise true.
         */
        template <class ParseContext>
        parse_result parse(ParseContext& pc) const noexcept {
            parse_result result = m_parser.parse(pc);
            if (!result) {
                return result;
            }
            return do_optional_parse_loop(m_parser, pc);
            return true;
        }

        /**
         * Same as parse(pc), but for the first step of left recursion parsing.
         * @param pc the parse context.
         * @return false if it fails the first time, otherwise true.
         */
        template <class ParseContext>
        parse_result parse_left_recursion_start(ParseContext& pc) const noexcept {
            parse_result result = m_parser.parse_left_recursion_start(pc);
            if (!result) {
                return result;
            }
            return do_optional_parse_loop(m_parser, pc);
        }

        /**
         * Same as parse(pc), but for the subsequent steps of left recursion parsing.
         * @param pc the parse context.
         * @param match_start start state of left recursion.
         * @return false if it fails the first time, otherwise true.
         */
        template <class ParseContext, class State>
        parse_result parse_left_recursion_continuation(ParseContext& pc, const State& match_start) const noexcept {
            parse_result result = m_parser.parse_left_recursion_continuation(pc, match_start);
            if (!result) {
                return result;
            }
            return do_optional_parse_loop(m_parser, pc);
        }

    private:
        Parser m_parser;
    };


    /**
     * A parse node that makes another parse node optional.
     * @param Parser the parse node to make optional.
     */
    template <class Parser>
    class optional_parse_node : public parse_node<optional_parse_node<Parser>> {
    public:
        /**
         * The constructor.
         * @param parser the parse node to make optional.
         */
        optional_parse_node(const Parser& parser) noexcept
            : m_parser(parser)
        {
        }

        /**
         * Uses the specified parse node to parse.
         * If the parse node returns false, then this parse node returns true.
         * @param pc the parse context.
         * @return either true or left recursion.
         */
        template <class ParseContext>
        parse_result parse(ParseContext& pc) const noexcept {
            parse_result result = m_parser.parse(pc);
            switch (result.value()) {
                case parse_result::FALSE:
                case parse_result::TRUE:
                    return true;
                case parse_result::LEFT_RECURSION:
                    return result;
            }
            return true;
        }

        /**
         * Same as parse(pc), but for the first step of left recursion parsing.
         * @param pc the parse context.
         * @return either true or left recursion.
         */
        template <class ParseContext>
        parse_result parse_left_recursion_start(ParseContext& pc) const noexcept {
            parse_result result = m_parser.parse_left_recursion_start(pc);
            switch (result.value()) {
                case parse_result::FALSE:
                case parse_result::TRUE:
                    return true;
                case parse_result::LEFT_RECURSION:
                    return result;
            }
            return true;
        }

        /**
         * Same as parse(pc), but for the subsequent steps of left recursion parsing.
         * @param pc the parse context.
         * @param match_start start state of left recursion.
         * @return either true or left recursion.
         */
        template <class ParseContext, class State>
        parse_result parse_left_recursion_continuation(ParseContext& pc, const State& match_start) const noexcept {
            parse_result result = m_parser.parse_left_recursion_continuation(pc, match_start);
            switch (result.value()) {
                case parse_result::FALSE:
                case parse_result::TRUE:
                    return true;
                case parse_result::LEFT_RECURSION:
                    return result;
            }
            return true;
        }

    private:
        Parser m_parser;
    };


    /**
     * A parse node that uses another parse node to parse, then restores the parse context state
     * to the one before the parsing, effectively allowing any parse node to work as a logical predicate.
     * @param Parser the parse node to use for testing.
     */
    template <class Parser>
    class logical_and_parse_node : public parse_node<logical_and_parse_node<Parser>> {
    public:
        /**
         * The constructor.
         * @param Parser the parse node to use for testing.
         */
        logical_and_parse_node(const Parser& parser) noexcept
            : m_parser(parser)
        {
        }

        /**
         * Uses the specified parser to parse, then returns its result,
         * restoring the parse context to the state before this call.
         * @param pc the parse context.
         * @return whatever the specified parser returns.
         */
        template <class ParseContext>
        parse_result parse(ParseContext& pc) const noexcept {
            const auto state = pc.state();
            parse_result result = m_parser.parse(pc);
            pc.set_state(state);
            return result;
        }

        /**
         * Same as parse(pc), but for the first step of left recursion parsing.
         * @param pc the parse context.
         * @return whatever the specified parser returns.
         */
        template <class ParseContext>
        parse_result parse_left_recursion_start(ParseContext& pc) const noexcept {
            const auto state = pc.state();
            parse_result result = m_parser.parse_left_recursion_start(pc);
            pc.set_state(state);
            return result;
        }

        /**
         * Same as parse(pc), but for the subsequent steps of left recursion parsing.
         * @param pc the parse context.
         * @param match_start start state of left recursion.
         * @return whatever the specified parser returns.
         */
        template <class ParseContext, class State>
        parse_result parse_left_recursion_continuation(ParseContext& pc, const State& match_start) const noexcept {
            const auto state = pc.state();
            parse_result result = m_parser.parse_left_recursion_continuation(pc, match_start);
            pc.set_state(state);
            return result;
        }

    private:
        Parser m_parser;
    };


    /**
     * A parse node that uses another parse node to parse, then restores the parse context state
     * to the one before the parsing, effectively allowing any parse node to work as a negating logical predicate.
     * @param Parser the parse node to use for testing.
     */
    template <class Parser>
    class logical_not_parse_node : public parse_node<logical_not_parse_node<Parser>> {
    public:
        /**
         * The constructor.
         * @param Parser the parse node to use for testing.
         */
        logical_not_parse_node(const Parser& parser) noexcept
            : m_parser(parser)
        {
        }

        /**
         * Uses the specified parser to parse, then returns the negation of its result,
         * restoring the parse context to the state before this call.
         * @param pc the parse context.
         * @return the negation of whatever the specified parser returns.
         */
        template <class ParseContext>
        parse_result parse(ParseContext& pc) const noexcept {
            const auto state = pc.state();
            parse_result result = m_parser.parse(pc);
            pc.set_state(state);
            switch (result.value()) {
                case parse_result::FALSE:
                    return true;
                case parse_result::TRUE:
                    return false;
                case parse_result::LEFT_RECURSION:
                    break;
            }
            return result;
        }

        /**
         * Same as parse(pc), but for the first step of left recursion parsing.
         * @param pc the parse context.
         * @return the negation of whatever the specified parser returns.
         */
        template <class ParseContext>
        parse_result parse_left_recursion_start(ParseContext& pc) const noexcept {
            const auto state = pc.state();
            parse_result result = m_parser.parse_left_recursion_start(pc);
            pc.set_state(state);
            switch (result.value()) {
                case parse_result::FALSE:
                    return true;
                case parse_result::TRUE:
                    return false;
            }
            return result;
        }

        /**
         * Same as parse(pc), but for the subsequent steps of left recursion parsing.
         * @param pc the parse context.
         * @param match_start start state of left recursion.
         * @return the negation of whatever the specified parser returns.
         */
        template <class ParseContext, class State>
        parse_result parse_left_recursion_continuation(ParseContext& pc, const State& match_start) const noexcept {
            const auto state = pc.state();
            parse_result result = m_parser.parse_left_recursion_continuation(pc, match_start);
            pc.set_state(state);
            switch (result.value()) {
                case parse_result::FALSE:
                    return true;
                case parse_result::TRUE:
                    return false;
            }
            return result;
        }

    private:
        Parser m_parser;
    };


    template <class Derived>
    auto parse_node<Derived>::operator *() const noexcept {
        return zero_or_more_parse_node<Derived>(static_cast<const Derived&>(*this));
    }


    template <class Derived>
    auto parse_node<Derived>::operator +() const noexcept {
        return one_or_more_parse_node<Derived>(static_cast<const Derived&>(*this));
    }


    template <class Derived>
    auto parse_node<Derived>::operator -() const noexcept {
        return optional_parse_node<Derived>(static_cast<const Derived&>(*this));
    }


    template <class Derived>
    auto parse_node<Derived>::operator &() const noexcept {
        return logical_and_parse_node<Derived>(static_cast<const Derived&>(*this));
    }


    template <class Derived>
    auto parse_node<Derived>::operator !() const noexcept {
        return logical_not_parse_node<Derived>(static_cast<const Derived&>(*this));
    }


} //namespace parserlib


#endif //PARSERLIB_UNARY_PARSE_NODES_HPP
