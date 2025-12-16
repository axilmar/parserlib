#ifndef PARSERLIB_DEBUG_PARSE_NODE_HPP
#define PARSERLIB_DEBUG_PARSE_NODE_HPP


#include "parent_parse_node.hpp"


namespace parserlib {


    /**
     * A parse node that can be used to debug another parse node.
     * @param Child Type of the parse node to debug.
     */
    template <class Child>
    class debug_parse_node : public parent_parse_node<debug_parse_node<Child>, Child> {
    public:
        /** The parent parse node type. */
        using parent_type = parent_parse_node<debug_parse_node<Child>, Child>;

        /**
         * The constructor.
         * @param child the child.
         */
        debug_parse_node(const Child& child)
            : parent_type(child)
        {
        }

        /**
         * It invokes the child node to parse.
         * A breakpoint can be put here for debugging.
         * @param pc the context to pass to the child.
         * @return the result of the child parse node.
         */
        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            const bool result = parent_type::get_children().parse(pc);
            return result;
        }
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
