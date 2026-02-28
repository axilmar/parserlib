#ifndef PARSERLIB_FUNCTION_PARSE_NODE_HPP
#define PARSERLIB_FUNCTION_PARSE_NODE_HPP


#include "parse_node.hpp"


namespace parserlib {


    template <class ParseContext, class F>
    class function_parse_node : public parse_node<ParseContext> {
    public:
        function_parse_node(const F& func)
            : m_function(func)
        {
        }

        bool parse(ParseContext& pc) const override {
            return m_function(pc);
        }

    private:
        F m_function;
    };


} //namespace parserlib


#endif //PARSERLIB_FUNCTION_PARSE_NODE_HPP
