#ifndef PARSERLIB_LOOP_1_PARSE_NODE_HPP
#define PARSERLIB_LOOP_1_PARSE_NODE_HPP


#include "parse_node.hpp"


namespace parserlib {


    /**
     * A parse node which invokes a child node 1 or more times.
     * @param ParseNode type of child parse node.
     */
    template <class ParseNode> 
    class loop_1_parse_node : public parse_node<loop_1_parse_node<ParseNode>> {
    public:
        /**
         * The constructor.
         * @param child the child parse node.
         */
        loop_1_parse_node(const ParseNode& child)
            : m_child(child)
        {
        }

        /**
         * It loops while the child parser returns true.
         * It aborts the loop if the child parser returns false
         * or the parse position remains the same after the parse.
         * @param pc the current parse context.
         * @return false if the child parser failed to parse at least once, otherwise true.
         */
        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            if (!m_child.parse(pc)) {
                return false;
            }
            while (pc.parse_valid()) {
                const auto start_iterator = pc.parse_position().iterator();
                if (!m_child.parse(pc) || pc.parse_position().iterator() == start_iterator) {
                    break;
                }
            }
            return true;
        }

    private:
        const ParseNode m_child;
    };


    template <class Derived>
    loop_1_parse_node<Derived> parse_node<Derived>::operator +() const {
        return *derived();
    }


} //namespace parserlib


#endif //PARSERLIB_LOOP_1_PARSE_NODE_HPP
