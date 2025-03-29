#ifndef PARSERLIB_OTHER_PARSER_NODES_HPP
#define PARSERLIB_OTHER_PARSER_NODES_HPP


#include "parse_node.hpp"


namespace parserlib {


    class bool_parse_node : public parse_node<bool_parse_node> {
    public:
        constexpr bool_parse_node(bool value) noexcept
            : m_value(value)
        {
        }

        template <class ParseContext>
        parse_result parse(ParseContext& pc) const noexcept {
            return m_value;
        }

        template <class ParseContext>
        parse_result parse_left_recursion_start(ParseContext& pc) const noexcept {
            return m_value;
        }

        template <class ParseContext, class State>
        parse_result parse_left_recursion_continuation(ParseContext& pc, const State& match_start) const noexcept {
            return m_value;
        }

    private:
        bool m_value;
    };


    inline bool_parse_node get_parse_node_wrapper(bool value) {
        return value;
    }


    inline constexpr bool_parse_node false_(false);
    inline constexpr bool_parse_node true_(true);


    class end_parse_node : public parse_node<end_parse_node> {
    public:
        template <class ParseContext>
        parse_result parse(ParseContext& pc) const noexcept {
            return pc.is_end_parse_position();
        }

        template <class ParseContext>
        parse_result parse_left_recursion_start(ParseContext& pc) const noexcept {
            return pc.is_end_parse_position();
        }

        template <class ParseContext, class State>
        parse_result parse_left_recursion_continuation(ParseContext& pc, const State& match_start) const noexcept {
            return pc.is_end_parse_position();
        }
    };


    inline constexpr end_parse_node end;


    template <class F>
    class function_parse_node : public parse_node<function_parse_node<F>> {
    public:
        function_parse_node(const F& f) noexcept
            : m_function(f)
        {
        }

        template <class ParseContext>
        parse_result parse(ParseContext& pc) const noexcept {
            if (pc.is_valid_parse_position() && m_function(pc)) {
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
        F m_function;
    };


    template <class F>
    auto function(const F& f) noexcept {
        return function_parse_node([f](auto& pc) { return f(pc); });
    }


} //namespace parserlib


#endif //PARSERLIB_OTHER_PARSER_NODES_HPP
