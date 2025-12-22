#ifndef PARSERLIB_LOGICAL_AND_PARSE_NODE_HPP
#define PARSERLIB_LOGICAL_AND_PARSE_NODE_HPP


#include "parse_node.hpp"


namespace parserlib {


    /**
     * A parse node that makes another parse node a logical and predicate.
     * @param Child the type of the child parse node.
     */
    template <class Child>
    class logical_and_parse_node : public parse_node<logical_and_parse_node<Child>> {
    public:
        /**
         * The constructor.
         * @param child the child parse node.
         */
        logical_and_parse_node(const Child& child)
            : m_child(child)
        {
        }

        /**
         * Invokes the child once, then returns true.
         * @param pc the parse context.
         * @return always true.
         */
        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            return pc.parse_and_restore_state([&]() { 
                return m_child.parse(pc);
            });
        }

    private:
        Child m_child;
    };


} //namespace parserlib


#endif //PARSERLIB_LOGICAL_AND_PARSE_NODE_HPP
