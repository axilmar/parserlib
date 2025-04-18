#ifndef PARSERLIB_OTHER_PARSER_NODES_HPP
#define PARSERLIB_OTHER_PARSER_NODES_HPP


#include "parse_node.hpp"


namespace parserlib {


    /**
     * A boolean parse node.
     */
    class bool_parse_node : public parse_node<bool_parse_node> {
    public:
        /**
         * The constructor.
         * @param value boolean value to return from the parse functions.
         */
        constexpr bool_parse_node(bool value) noexcept
            : m_value(value)
        {
        }

        /**
         * Returns the boolean value.
         * @param pc the parse context.
         * @return the specified boolean value.
         */
        template <class ParseContext>
        parse_result parse(ParseContext& pc) const noexcept {
            return m_value;
        }

        /**
         * Same as parse(pc), but for the first step of left recursion parsing.
         * @param pc the parse context.
         * @return the specified boolean value.
         */
        template <class ParseContext>
        parse_result parse_left_recursion_start(ParseContext& pc) const noexcept {
            return m_value;
        }

        /**
         * Same as parse(pc), but for the subsequent steps of left recursion parsing.
         * @param pc the parse context.
         * @param match_start start state of left recursion.
         * @return the specified boolean value.
         */
        template <class ParseContext, class State>
        parse_result parse_left_recursion_continuation(ParseContext& pc, const State& match_start) const noexcept {
            return m_value;
        }

    private:
        bool m_value;
    };


    /**
     * Overload of the function for boolean values.
     * @param value the boolean value.
     * @return a boolean parse node.
     */
    inline bool_parse_node get_parse_node_wrapper(bool value) noexcept {
        return value;
    }


    /**
     * A parse node that calls a user function for parsing.
     * @param F the function to use for parsing; it must have the signature: 
     *  template <class ParseContext> parse_result (ParseContext& pc) noexcept.
     */
    template <class F>
    class function_parse_node : public parse_node<function_parse_node<F>> {
    public:
        /**
         * The constructor.
         */
        function_parse_node(const F& f) noexcept
            : m_function(f)
        {
        }

        /**
         * Invokes the specified function.
         * @param pc the parse context.
         * @return the function's result or false if the end of input has been reached.
         */
        template <class ParseContext>
        parse_result parse(ParseContext& pc) const noexcept {
            if (pc.is_valid_parse_position()) {
                return m_function(pc);
            }
            return false;
        }

        /**
         * Same as parse(pc), but for the first step of left recursion parsing.
         * @param pc the parse context.
         * @return the function's result or false if the end of input has been reached.
         */
        template <class ParseContext>
        parse_result parse_left_recursion_start(ParseContext& pc) const noexcept {
            return parse(pc);
        }

        /**
         * Same as parse(pc), but for the subsequent steps of left recursion parsing.
         * @param pc the parse context.
         * @param match_start start state of left recursion.
         * @return the function's result or false if the end of input has been reached.
         */
        template <class ParseContext, class State>
        parse_result parse_left_recursion_continuation(ParseContext& pc, const State& match_start) const noexcept {
            return false;
        }

    private:
        F m_function;
    };


    /**
     * Helper function for creating a function parse node from of a functor or a function.
     * @param f functor.
     * @return a function parse node for the functor or function.
     */
    template <class F>
    auto function(const F& f) noexcept {
        return function_parse_node<F>(f);
    }


    /**
     * Overload for pointer to function.
     * @param f pointer to function (with noexcept specification).
     * @return a function parse node that uses the given function to test the current token.
     */
    template <class R, class T>
    auto get_parse_node_wrapper(R(*f)(T) noexcept) noexcept {
        const auto l = [f](auto& pc) noexcept { 
            if (f(*pc.parse_position())) {
                pc.increment_parse_position();
                return true;
            }
            return false;
        };
        return function_parse_node<decltype(l)>(l);
    }


    /**
     * Overload for pointer to function.
     * @param f pointer to function (without noexcept specification).
     * @return a function parse node that uses the given function to test the current token.
     */
    template <class R, class T>
    auto get_parse_node_wrapper(R(*f)(T)) noexcept {
        const auto l = [f](auto& pc) noexcept {
            if (f(*pc.parse_position())) {
                pc.increment_parse_position();
                return true;
            }
            return false;
        };
        return function_parse_node<decltype(l)>(l);
    }


    /**
     * A parse node that invokes a callback when a parser parses an input.
     * @param Parser the parser to invoke for parsing.
     * @param Callback the callback t invoke with the parser's result. It must have the following signature:
     *  template <class ParseContext, class Iterator> 
     *      parse_result (ParseConext& pc, const parse_result& parser_result, const Iterator& begin, const Iterator& end);
     * The parameters of the callback function are:
     *  - pc: the parse context.
     *  - parser_result: the result of the Parser class.
     *  - begin: parse position before the parser is invoked.
     *  - end: parse position after the parser is invoked.
     */
    template <class Parser, class Callback>
    class callback_parse_node : public parse_node<callback_parse_node<Parser, Callback>> {
    public:
        /**
         * The constructor.
         * @param parser the parser to invoke.
         * @param callback the callback to invoke after the parser.
         */
        callback_parse_node(const Parser& parser, const Callback& callback)
            : m_parser(parser)
            , m_callback(callback)
        {
        }

        /**
         * Invokes the parser, then invokes the callback with the result and the parsed input range.
         * @param pc the parse context.
         * @return whatever the callback returns.
         */
        template <class ParseContext>
        parse_result parse(ParseContext& pc) const noexcept {
            const auto start = pc.parse_position();
            parse_result result = m_parser.parse(pc);
            return m_callback(pc, result, start, pc.parse_position());
        }

        /**
         * Same as parse(pc), but for the first step of left recursion parsing.
         * @param pc the parse context.
         * @return whatever the callback returns.
         */
        template <class ParseContext>
        parse_result parse_left_recursion_start(ParseContext& pc) const noexcept {
            const auto start = pc.parse_position();
            parse_result result = m_parser.parse_left_recursion_start(pc);
            return m_callback(pc, result, start, pc.parse_position());
        }

        /**
         * Same as parse(pc), but for the subsequent steps of left recursion parsing.
         * @param pc the parse context.
         * @param match_start start state of left recursion.
         * @return whatever the callback returns.
         */
        template <class ParseContext, class State>
        parse_result parse_left_recursion_continuation(ParseContext& pc, const State& match_start) const noexcept {
            parse_result result = m_parser.parse_left_recursion_continuation(pc, match_start);
            return m_callback(pc, result, match_start.position(), pc.parse_position());
        }

    private:
        Parser m_parser;
        Callback m_callback;
    };


    /**
     * A helper class used for debugging.
     * Provided so as that breakpoints can be added before a parser is invoked to parse.
     * @param Parser the class to invoke for parsing.
     */
    template <class Parser>
    class debug_parse_node : public parse_node<debug_parse_node<Parser>> {
    public:
        /**
         * The constructor.
         * @param parser the parser to invoke.
         */
        debug_parse_node(const Parser& parser) noexcept
            : m_parser(parser)
        {
        }

        /**
         * Invokes the parser to parse.
         * @param pc the parse context.
         * @return whatever the parser returns.
         */
        template <class ParseContext>
        parse_result parse(ParseContext& pc) const noexcept {
            const parse_result result = m_parser.parse(pc);
            return result;
        }

        /**
         * Same as parse(pc), but for the first step of left recursion parsing.
         * @param pc the parse context.
         * @return whatever the parser returns.
         */
        template <class ParseContext>
        parse_result parse_left_recursion_start(ParseContext& pc) const noexcept {
            const parse_result result = m_parser.parse_left_recursion_start(pc);
            return result;
        }

        /**
         * Same as parse(pc), but for the subsequent steps of left recursion parsing.
         * @param pc the parse context.
         * @return whatever the parser returns.
         */
        template <class ParseContext, class State>
        parse_result parse_left_recursion_continuation(ParseContext& pc, const State& match_start) const noexcept {
            const parse_result result = m_parser.parse_left_recursion_continuation(pc, match_start);
            return result;
        }

    private:
        Parser m_parser;
    };


    /**
     * Helper class that creates a debug parse node.
     * @param parser the parser to invoke.
     * @return a debug parse node.
     */
    template <class Parser>
    auto debug(Parser&& parser) noexcept {
        return debug_parse_node(get_parse_node_wrapper(std::forward<Parser>(parser)));
    }


    /**
     * A parse node that parses any terminal. 
     */
    class any_parse_node : public parse_node<any_parse_node> {
    public:
        /**
         * Checks if the parse position is valid, and then increments it.
         * @param pc the parse context.
         * @return true if the parse position was valid, false otherwise.
         */
        template <class ParseContext>
        parse_result parse(ParseContext& pc) const noexcept {
            if (pc.is_valid_parse_position()) {
                pc.increment_parse_position();
                return true;
            }
            return false;
        }

        /**
         * Same as parse(pc), but for the first step of left recursion parsing.
         * @param pc the parse context.
         * @return whatever the parser returns.
         */
        template <class ParseContext>
        parse_result parse_left_recursion_start(ParseContext& pc) const noexcept {
            return parse(pc);
        }

        /**
         * Same as parse(pc), but for the subsequent steps of left recursion parsing.
         * @param pc the parse context.
         * @return whatever the parser returns.
         */
        template <class ParseContext, class State>
        parse_result parse_left_recursion_continuation(ParseContext& pc, const State& match_start) const noexcept {
            return parse(pc);
        }
    };


    /**
     * Helper function for creating an any_parse_node instance.
     * @return an any_parse_node instance.
     */
    inline any_parse_node any() noexcept {
        return {};
    }


    /**
     * A parse node that parses the end of input.
     */
    class end_parse_node : public parse_node<end_parse_node> {
    public:
        /**
         * Checks if the end of input has been reached.
         * @param pc the parse context.
         * @return true if the end of input has been reached, false otherwise.
         */
        template <class ParseContext>
        parse_result parse(ParseContext& pc) const noexcept {
            return pc.is_end_parse_position();
        }

        /**
         * Same as parse(pc), but for the first step of left recursion parsing.
         * @param pc the parse context.
         * @return whatever the parser returns.
         */
        template <class ParseContext>
        parse_result parse_left_recursion_start(ParseContext& pc) const noexcept {
            return parse(pc);
        }

        /**
         * Same as parse(pc), but for the subsequent steps of left recursion parsing.
         * @param pc the parse context.
         * @return whatever the parser returns.
         */
        template <class ParseContext, class State>
        parse_result parse_left_recursion_continuation(ParseContext& pc, const State& match_start) const noexcept {
            return parse(pc);
        }
    };


    /**
     * Helper function for creating an end_parse_node instance.
     * @return an end_parse_node instance.
     */
    inline end_parse_node end() noexcept {
        return {};
    }


} //namespace parserlib


#endif //PARSERLIB_OTHER_PARSER_NODES_HPP
