#ifndef PARSERLIB_OPTIONAL_PARSE_NODE_HPP
#define PARSERLIB_OPTIONAL_PARSE_NODE_HPP


#include "parse_node.hpp"


namespace parserlib {


    /**
     * A parse node which invokes a child node and disregards the result.
     * @param ParseNode type of child parse node.
     */
    template <class ParseNode> 
    class optional_parse_node : public parse_node<optional_parse_node<ParseNode>> {
    public:
        /**
         * The constructor.
         * @param child the child parse node.
         */
        optional_parse_node(const ParseNode& child)
            : m_child(child)
            #ifndef NDEBUG
            , m_text("-(" + m_child.text() + ")")
            #endif
        {
        }

        /**
         * It invokes the child parse node.
         * @param pc the current parse context.
         * @return always true.
         */
        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            const auto initial_state = pc.get_state();
            if (m_child.parse(pc)) {
                return true;
            }
            pc.set_state(initial_state);
            return true;
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
    optional_parse_node<Derived> parse_node<Derived>::operator -() const {
        return *derived();
    }


} //namespace parserlib


#endif //PARSERLIB_OPTIONAL_PARSE_NODE_HPP
