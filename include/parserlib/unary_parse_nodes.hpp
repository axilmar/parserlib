#ifndef PARSERLIB_UNARY_PARSE_NODES_HPP
#define PARSERLIB_UNARY_PARSE_NODES_HPP


#include "parse_node.hpp"


namespace parserlib {


    template <class Parser>
    class zero_or_more_parse_node : public parse_node<zero_or_more_parse_node<Parser>> {
    public:
        zero_or_more_parse_node(const Parser& parser) noexcept
            : m_parser(parser)
        {
        }

        template <class ParseContext>
        parse_result parse(ParseContext& pc) const noexcept {
            for (;;) {
                parse_result result = m_parser.parse(pc);
                switch (result.value()) {
                    case parse_result::FALSE:
                        return true;
                    case parse_result::TRUE:
                        continue;
                    case parse_result::LEFT_RECURSION:
                        return result;
                }
            }
            return true;
        }

        template <class ParseContext>
        parse_result parse_left_recursion_start(ParseContext& pc) const noexcept {
            parse_result result = m_parser.parse_left_recursion_start(pc);
            switch (result.value()) {
                case parse_result::FALSE:
                    return true;
                case parse_result::LEFT_RECURSION:
                    return result;
            }
            for (;;) {
                parse_result result = m_parser.parse(pc);
                switch (result.value()) {
                    case parse_result::FALSE:
                        return true;
                    case parse_result::TRUE:
                        continue;
                    case parse_result::LEFT_RECURSION:
                        return result;
                }
            }
            return true;
        }

        template <class ParseContext, class State>
        parse_result parse_left_recursion_continuation(ParseContext& pc, const State& match_start) const noexcept {
            parse_result result = m_parser.parse_left_recursion_continuation(pc, match_start);
            switch (result.value()) {
                case parse_result::FALSE:
                    return true;
                case parse_result::LEFT_RECURSION:
                    return result;
            }
            for (;;) {
                parse_result result = m_parser.parse(pc);
                switch (result.value()) {
                    case parse_result::FALSE:
                        return true;
                    case parse_result::TRUE:
                        continue;
                    case parse_result::LEFT_RECURSION:
                        return result;
                }
            }
            return true;
        }

    private:
        Parser m_parser;
    };


    template <class Parser>
    class one_or_more_parse_node : public parse_node<one_or_more_parse_node<Parser>> {
    public:
        one_or_more_parse_node(const Parser& parser) noexcept
            : m_parser(parser)
        {
        }

        template <class ParseContext>
        parse_result parse(ParseContext& pc) const noexcept {
            parse_result result = m_parser.parse(pc);
            if (!result) {
                return result;
            }
            for (;;) {
                parse_result result = m_parser.parse(pc);
                switch (result.value()) {
                    case parse_result::FALSE:
                        return true;
                    case parse_result::TRUE:
                        continue;
                    case parse_result::LEFT_RECURSION:
                        return result;
                    }
            }
            return true;
        }

        template <class ParseContext>
        parse_result parse_left_recursion_start(ParseContext& pc) const noexcept {
            parse_result result = m_parser.parse_left_recursion_start(pc);
            if (!result) {
                return result;
            }
            for (;;) {
                parse_result result = m_parser.parse(pc);
                switch (result.value()) {
                    case parse_result::FALSE:
                        return true;
                    case parse_result::TRUE:
                        continue;
                    case parse_result::LEFT_RECURSION:
                        return result;
                }
            }
            return true;
        }

        template <class ParseContext, class State>
        parse_result parse_left_recursion_continuation(ParseContext& pc, const State& match_start) const noexcept {
            parse_result result = m_parser.parse_left_recursion_continuation(pc, match_start);
            if (!result) {
                return result;
            }
            for (;;) {
                parse_result result = m_parser.parse(pc);
                switch (result.value()) {
                    case parse_result::FALSE:
                        return true;
                    case parse_result::TRUE:
                        continue;
                    case parse_result::LEFT_RECURSION:
                        return result;
                }
            }
            return true;
        }

    private:
        Parser m_parser;
    };


    template <class Parser>
    class optional_parse_node : public parse_node<optional_parse_node<Parser>> {
    public:
        optional_parse_node(const Parser& parser) noexcept
            : m_parser(parser)
        {
        }

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


    template <class Parser>
    class logical_and_parse_node : public parse_node<logical_and_parse_node<Parser>> {
    public:
        logical_and_parse_node(const Parser& parser) noexcept
            : m_parser(parser)
        {
        }

        template <class ParseContext>
        parse_result parse(ParseContext& pc) const noexcept {
            const auto state = pc.state();
            parse_result result = m_parser.parse(pc);
            pc.set_state(state);
            return result;
        }

        template <class ParseContext>
        parse_result parse_left_recursion_start(ParseContext& pc) const noexcept {
            const auto state = pc.state();
            parse_result result = m_parser.parse_left_recursion_start(pc);
            pc.set_state(state);
            return result;
        }

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


    template <class Parser>
    class logical_not_parse_node : public parse_node<logical_not_parse_node<Parser>> {
    public:
        logical_not_parse_node(const Parser& parser) noexcept
            : m_parser(parser)
        {
        }

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
            }
            return result;
        }

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


} //namespace parserlib


#endif //PARSERLIB_UNARY_PARSE_NODES_HPP
