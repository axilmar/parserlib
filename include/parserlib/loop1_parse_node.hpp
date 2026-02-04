#ifndef PARSERLIB_LOOP1_PARSE_NODE_HPP
#define PARSERLIB_LOOP1_PARSE_NODE_HPP


#include "parse_node.hpp"
#include "parse_algorithms.hpp"


namespace parserlib {


    /**
     * A parse node that makes another parse node be repeated
     * at least once.
     * @param Child the type of the child parse node.
     */
    template <class Child>
    class loop1_parse_node : public parse_node<loop1_parse_node<Child>> {
    public:
        /**
         * The constructor.
         * @param child the child parse node.
         */
        loop1_parse_node(const Child& child)
            : m_child(child)
        {
        }

        /**
         * Invokes the child once and if the child succeeds,
         * then it invokes the child in a 'while' loop,
         * until the child cannot parse any more.
         * @param pc the parse context.
         * @return always true.
         */
        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            if (m_child.parse(pc)) {
                parse_loop_0(pc, [&]() { return m_child.parse(pc); });
                return true;
            }
            return false;
        }

    private:
        Child m_child;
    };


} //namespace parserlib


#endif //PARSERLIB_LOOP1_PARSE_NODE_HPP
