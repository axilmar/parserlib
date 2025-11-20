#ifndef PARSERLIB_LOOP_0_PARSE_NODE_HPP
#define PARSERLIB_LOOP_0_PARSE_NODE_HPP


#include "loop_functions.hpp"


namespace parserlib {


    /**
     * A parse node which invokes a child node 0 or more times.
     * @param ParseNode type of child parse node.
     */
    template <class ParseNode> 
    class loop_0_parse_node : public parse_node<loop_0_parse_node<ParseNode>> {
    public:
        /**
         * The constructor.
         * @param child the child parse node.
         */
        loop_0_parse_node(const ParseNode& child)
            : m_child(child)
        {
        }

        /**
         * It loops while the child parser returns true.
         * It aborts the loop if the child parser returns false
         * or the parse position remains the same after the parse.
         * @param pc the current parse context.
         * @return always true.
         */
        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            return loop_parse(pc, m_child);
        }

        /**
         * Converts the parse node to a textual description.
         * @return a string of this parse node as text.
         */
        std::string text() const override {
            return "*" + m_child.text();
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
    loop_0_parse_node<Derived> parse_node<Derived>::operator *() const {
        return *derived();
    }


} //namespace parserlib


#endif //PARSERLIB_LOOP_0_PARSE_NODE_HPP
