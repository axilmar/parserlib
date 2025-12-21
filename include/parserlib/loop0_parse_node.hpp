#ifndef PARSERLIB_LOOP0_PARSE_NODE_HPP
#define PARSERLIB_LOOP0_PARSE_NODE_HPP


#include "parse_node.hpp"


namespace parserlib {


    /**
     * A parse node that makes another parse node be repeated
     * until no more parsing can be done.
     * @param Child the type of the child parse node.
     */
    template <class Child>
    class loop0_parse_node : public parse_node<loop0_parse_node<Child>> {
    public:
        /**
         * The constructor.
         * @param child the child parse node.
         */
        loop0_parse_node(const Child& child)
            : m_child(child)
        {
        }

        /**
         * Invokes the child in a 'while' loop,
         * until the child cannot parse any more.
         * @param pc the parse context.
         * @return always true.
         */
        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            while (m_child.parse(pc)) {
            }
            return true;
        }

    private:
        Child m_child;
    };


} //namespace parserlib


#endif //PARSERLIB_LOOP0_PARSE_NODE_HPP
