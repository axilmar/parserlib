#ifndef PARSERLIB_MEMOIZE_PARSE_NODE_HPP
#define PARSERLIB_MEMOIZE_PARSE_NODE_HPP


#include "parse_node.hpp"


namespace parserlib {


    /**
     * A parse node that remembers the parsing result for a particular grammar.
     * @param Child the type of the child parse node.
     */
    template <class Child>
    class memoize_parse_node : public parse_node<memoize_parse_node<Child>> {
    public:
        /**
         * The constructor.
         * @param child the child parse node.
         */
        memoize_parse_node(const Child& child)
            : m_child(child)
        {
        }

        /**
         * If the child node is not invoked yet for the current position, then it is invoked.
         * Otherwise, the result is recalled from previous parsing.
         * @param pc the parse context.
         * @return always true.
         */
        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            return pc.parse_with_memoization(m_child);
        }

    private:
        Child m_child;
    };


    /**
     * Helper function for creating a memoisation parse node.
     * @param expr parse node/expression to create a memoisation parse node for.
     */ 
    template <class T>
    auto memoize(const T& expr) {
        return memoize_parse_node{ make_parse_node(expr) };
    }


} //namespace parserlib


#endif //PARSERLIB_MEMOIZE_PARSE_NODE_HPP
