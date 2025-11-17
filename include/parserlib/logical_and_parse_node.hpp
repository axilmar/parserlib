#ifndef PARSERLIB_LOGICAL_AND_PARSE_NODE_HPP
#define PARSERLIB_LOGICAL_AND_PARSE_NODE_HPP


#include "parse_node.hpp"


namespace parserlib {


    /**
     * A parse node which invokes a child node, then restores the parse context state,
     * returning the result of the child node.
     * It can be used as a logical predicate.
     * @param ParseNode type of child parse node.
     */
    template <class ParseNode>
    class logical_and_parse_node : public parse_node<logical_and_parse_node<ParseNode>> {
    public:
        /**
         * The constructor.
         * @param child the child parse node.
         */
        logical_and_parse_node(const ParseNode& child)
            : m_child(child)
            #ifndef NDEBUG
            , m_text("&(" + m_child.text() + ")")
            #endif
        {
        }

        /**
         * It invokes the child parse node.
         * @param pc the current parse context; on return, it is in the same state as on entrance.
         * @return the result of the child parse node.
         */
        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            const auto state = pc.get_state();
            const bool result = m_child.parse(pc);
            pc.set_state(state);
            return result;
        }

        #ifndef NDEBUG
        const std::string& text() const {
            return m_text;
        }
        #endif

    private:
        const ParseNode m_child;
        #ifndef NDEBUG
        const std::string m_text;
        #endif
    };


    template <class Derived>
    logical_and_parse_node<Derived> parse_node<Derived>::operator &() const {
        return *derived();
    }


} //namespace parserlib


#endif //PARSERLIB_LOGICAL_AND_PARSE_NODE_HPP
