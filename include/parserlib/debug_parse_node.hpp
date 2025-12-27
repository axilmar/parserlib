#ifndef PARSERLIB_DEBUG_PARSE_NODE_HPP
#define PARSERLIB_DEBUG_PARSE_NODE_HPP


#include "parse_node.hpp"


namespace parserlib {


    /**
     * A parse node that can be used to debug another parse node.
     * @param Child Type of the parse node to debug.
     */
    template <class Child>
    class debug_parse_node : public parse_node<debug_parse_node<Child>> {
    public:
        /**
         * The constructor.
         * @param child the child.
         */
        debug_parse_node(const Child& child)
            : m_child(child)
        {
        }

        /**
         * It invokes the child node to parse.
         * A breakpoint can be placed here for debugging.
         * @param pc the context to pass to the child.
         * @return the result of the child parse node.
         */
        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            //place breakpoint here
            const bool result = m_child.parse(pc);
            return result;
        }

    private:
        Child m_child;
    };


    /**
     * Creates a debug parse node for another parse node.
     * @param child the child parse node to debug.
     * @return a debug parse node.
     */
    template <class Child>
    debug_parse_node<Child> debug(const parse_node<Child>& child) {
        return *child.get_impl();
    }


} //namespace parserlib


#endif //PARSERLIB_DEBUG_PARSE_NODE_HPP
