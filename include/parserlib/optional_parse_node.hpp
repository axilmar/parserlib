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
            if (pc.parse(m_child)) {
                return true;
            }
            pc.set_state(initial_state);
            return true;
        }

        /**
         * Converts the parse node to a textual description.
         * @return a string of this parse node as text.
         */
        std::string text() const override {
            return "-" + m_child.text() + "";
        }

        #ifndef NDEBUG
        void init_tree() const override {
            m_child.init();
        }
        #endif

    private:
        const ParseNode m_child;
    };


    template <class Derived>
    optional_parse_node<Derived> parse_node<Derived>::operator -() const {
        return *derived();
    }


} //namespace parserlib


#endif //PARSERLIB_OPTIONAL_PARSE_NODE_HPP
