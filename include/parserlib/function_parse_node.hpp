#ifndef PARSERLIB_FUNCTION_PARSE_NODE_HPP
#define PARSERLIB_FUNCTION_PARSE_NODE_HPP


#include "parse_node.hpp"


namespace parserlib {


    /**
     * A parse node that invokes a function to parse.
     * @param F type of function to invoke.
     */
    template <class F>
    class function_parse_node : public parse_node<function_parse_node<F>> {
    public:
        /**
         * The constructor.
         * @param f the function to invoke.
         */
        function_parse_node(const F& function)
            : m_function(function)
        {
        }

        /**
         * Invokes the supplied function with the given parse context.
         * @param pc the parse context.
         * @return the result of the function.
         */
        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            return m_function(pc);
        }

    private:
        F m_function;
    };


    /**
     * Creates a function parse node.
     * @param function function to invoke for parsing.
     * @return the function parse node.
     */
    template <class F>
    function_parse_node<F> function(const F& function) {
        return function;
    }


} //namespace parserlib


#endif //PARSERLIB_FUNCTION_PARSE_NODE_HPP
