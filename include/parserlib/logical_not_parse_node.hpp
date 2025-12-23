#ifndef PARSERLIB_LOGICAL_NOT_PARSE_NODE_HPP
#define PARSERLIB_LOGICAL_NOT_PARSE_NODE_HPP


#include "parse_node.hpp"
#include "parse_algorithms.hpp"


namespace parserlib {


    /**
     * A parse node that makes another parse node a logical not predicate.
     * @param Child the type of the child parse node.
     */
    template <class Child>
    class logical_not_parse_node : public parse_node<logical_not_parse_node<Child>> {
    public:
        /**
         * The constructor.
         * @param child the child parse node.
         */
        logical_not_parse_node(const Child& child)
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
            return parse_and_restore_state(pc, [&]() { 
                return !m_child.parse(pc);
            });
        }

    private:
        Child m_child;
    };


} //namespace parserlib


#endif //PARSERLIB_LOGICAL_NOT_PARSE_NODE_HPP
