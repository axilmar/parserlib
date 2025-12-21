#ifndef PARSERLIB_OPTIONAL_PARSE_NODE_HPP
#define PARSERLIB_OPTIONAL_PARSE_NODE_HPP


#include "parse_node.hpp"


namespace parserlib {


    /**
     * A parse node that makes another parse node optional.
     * @param Child the type of the child parse node.
     */
    template <class Child>
    class optional_parse_node : public parse_node<optional_parse_node<Child>> {
    public:
        /**
         * The constructor.
         * @param child the child parse node.
         */
        optional_parse_node(const Child& child)
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
            m_child.parse(pc);
            return true;
        }

    private:
        Child m_child;
    };


} //namespace parserlib


#endif //PARSERLIB_OPTIONAL_PARSE_NODE_HPP
