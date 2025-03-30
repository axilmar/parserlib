#ifndef PARSERLIB_ERROR_HANDLING_PARSE_NODES_HPP
#define PARSERLIB_ERROR_HANDLING_PARSE_NODES_HPP


#include "parse_node.hpp"


namespace parserlib {


    // ERROR SKIP POLICIES ----------------------------------------------------


    class skip_policy_base {};


    template <class Parser>
    class skip_until_parse_node : public parse_node<skip_until_parse_node<Parser>>, public skip_policy_base {
    public:
        skip_until_parse_node(const Parser& parser) noexcept
            : m_parser(parser)
        {
        }

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


    template <class Parser>
    auto skip_until(Parser&& parser) {
        return skip_until_parse_node(get_parse_node_wrapper(std::forward<Parser>(parser)));
    }


    template <class Parser>
    class skip_until_after_parse_node : public parse_node<skip_until_after_parse_node<Parser>>, public skip_policy_base {
    public:
        skip_until_after_parse_node(const Parser& parser) noexcept
            : m_parser(parser)
        {
        }

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


    template <class Parser>
    auto skip_until_after(Parser&& parser) {
        return skip_until_after_parse_node(get_parse_node_wrapper(std::forward<Parser>(parser)));
    }


    template <class Parser>
    class skip_while_parse_node : public parse_node<skip_while_parse_node<Parser>>, public skip_policy_base {
    public:
        skip_while_parse_node(const Parser& parser) noexcept
            : m_parser(parser)
        {
        }

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


    template <class Parser>
    auto skip_while(Parser&& parser) {
        return skip_while_parse_node(get_parse_node_wrapper(std::forward<Parser>(parser)));
    }


    class skip_count_parse_node : public parse_node<skip_count_parse_node>, public skip_policy_base {
    public:
        skip_count_parse_node(std::size_t count) noexcept
            : m_count(count)
        {
        }

        template <class ParseContext>
        parse_result parse(ParseContext& pc) const noexcept {
            for (std::size_t count = m_count; pc.is_valid_parse_position() && count > 0; --count) {
                pc.increment_parse_position();
            }
            return true;
        }

        template <class ParseContext>
        parse_result parse_left_recursion_start(ParseContext& pc) const noexcept {
            return parse(pc);
        }

        template <class ParseContext, class State>
        parse_result parse_left_recursion_continuation(ParseContext& pc, const State& match_start) const noexcept {
            return parse(pc);
        }

    private:
        std::size_t m_count;
    };


    inline skip_count_parse_node skip_count(std::size_t count) {
        return count;
    }


    inline skip_count_parse_node skip_current() {
        return 1;
    }


    // ERROR HANDLER ----------------------------------------------------------


    template <class ErrorId>
    class error_handler_parse_node : public parse_node<error_handler_parse_node<ErrorId>> {
    public:
        using error_id_type = ErrorId;

        error_handler_parse_node(const error_id_type& error_id) noexcept
            : m_error_id(error_id)
        {
        }

        template <class ParseContext>
        parse_result parse(ParseContext& pc) const noexcept {
            const auto error_start = pc.parse_position();
            const auto error_end = pc.parse_position();
            pc.add_error(m_error_id, error_start, error_end);
            return true;
        }

        template <class ParseContext>
        parse_result parse_left_recursion_start(ParseContext& pc) const noexcept {
            return parse(pc);
        }

        template <class ParseContext, class State>
        parse_result parse_left_recursion_continuation(ParseContext& pc, const State& match_start) const noexcept {
            return parse(pc);
        }

    private:
        error_id_type m_error_id;
    };


    template <class ErrorId>
    auto error(const ErrorId& error_id) noexcept {
        return error_handler_parse_node(error_id);
    }


    // ERROR MATCH ------------------------------------------------------------


    template <class MatchId>
    class error_match_parse_node : public parse_node<error_match_parse_node<MatchId>> {
    public:
        error_match_parse_node(const MatchId& match_id) noexcept
            : m_match_id(match_id)
        {
        }

        template <class ParseContext>
        parse_result parse(ParseContext& pc) const noexcept {
            pc.add_match(m_match_id, pc.parse_position(), pc.parse_position());
            return true;
        }

        template <class ParseContext>
        parse_result parse_left_recursion_start(ParseContext& pc) const noexcept {
            return parse(pc);
        }

        template <class ParseContext, class State>
        parse_result parse_left_recursion_continuation(ParseContext& pc, const State& match_start) const noexcept {
            return parse(pc);
        }

    private:
        MatchId m_match_id;
    };


    template <class MatchId>
    auto error_match(const MatchId& match_id) noexcept {
        return error_match_parse_node(match_id);
    }


} //namespace parserlib


#endif //PARSERLIB_ERROR_HANDLING_PARSE_NODES_HPP
