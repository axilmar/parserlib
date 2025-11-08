#ifndef PARSERLIB_DEBUG_PARSE_NODE_HPP
#define PARSERLIB_DEBUG_PARSE_NODE_HPP


#include "parse_node.hpp"


namespace parserlib {


    /**
     * A parse node that can be used for setting a breakpoint,
     * before another parse node is invoked to parse.
     * @param ParseNode type of child parse node.
     */
    template <class ParseNode>
    class debug_parse_node : public parse_node<debug_parse_node<ParseNode>> {
    public:
        /**
         * The constructor.
         * @param child the child parse node.
         */
        debug_parse_node(const ParseNode& child)
            : m_child(child)
        {
        }

        /**
         * It invokes the child parse node.
         * @param pc the current parse context.
         * @return true for success, false on failure.
         */
        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            //put breakpoint here
            const bool result = m_child.parse(pc);
            return result;
        }

    private:
        const ParseNode m_child;
    };


    /**
     * Function that returns a debug parse node.
     * @param parse_node the parse node to use for debugging.
     * @return a debug parse node.
     */
    template <class ParseNode> 
    auto debug(ParseNode&& parse_node) {
        return debug_parse_node(make_parse_node(parse_node));
    }


} //namespace parserlib


#endif //PARSERLIB_DEBUG_PARSE_NODE_HPP
