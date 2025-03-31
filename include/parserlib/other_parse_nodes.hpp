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
        return function_parse_node(f);
    }


    template <class F>
    auto function(const F* f) noexcept {
        return function_parse_node([f](auto& pc) { return f(pc); });
    }


    template <class Parser, class Callback>
    class callback_parse_node : public parse_node<callback_parse_node<Parser, Callback>> {
    public:
        callback_parse_node(const Parser& parser, const Callback& callback)
            : m_parser(parser)
            , m_callback(callback)
        {
        }

        template <class ParseContext>
        parse_result parse(ParseContext& pc) const noexcept {
            const auto start = pc.parse_position();
            parse_result result = m_parser.parse(pc);
            return m_callback(pc, result, start, pc.parse_position());
        }

        template <class ParseContext>
        parse_result parse_left_recursion_start(ParseContext& pc) const noexcept {
            const auto start = pc.parse_position();
            parse_result result = m_parser.parse_left_recursion_start(pc);
            return m_callback(pc, result, start, pc.parse_position());
        }

        template <class ParseContext, class State>
        parse_result parse_left_recursion_continuation(ParseContext& pc, const State& match_start) const noexcept {
            parse_result result = m_parser.parse_left_recursion_continuation(pc, match_start);
            return m_callback(pc, result, match_start.position(), pc.parse_position());
        }

    private:
        Parser m_parser;
        Callback m_callback;
    };


    template <class Parser>
    class debug_parse_node : public parse_node<debug_parse_node<Parser>> {
    public:
        debug_parse_node(const Parser& parser) noexcept
            : m_parser(parser)
        {
        }

        template <class ParseContext>
        parse_result parse(ParseContext& pc) const noexcept {
            const parse_result result = m_parser.parse(pc);
            return result;
        }

        template <class ParseContext>
        parse_result parse_left_recursion_start(ParseContext& pc) const noexcept {
            const parse_result result = m_parser.parse_left_recursion_start(pc);
            return result;
        }

        template <class ParseContext, class State>
        parse_result parse_left_recursion_continuation(ParseContext& pc, const State& match_start) const noexcept {
            const parse_result result = m_parser.parse_left_recursion_continuation(pc, match_start);
            return result;
        }

    private:
        Parser m_parser;
    };


    template <class T>
    auto debug(T&& t) noexcept {
        return debug_parse_node(get_parse_node_wrapper(std::forward<T>(t)));
    }


} //namespace parserlib


#endif //PARSERLIB_OTHER_PARSER_NODES_HPP
