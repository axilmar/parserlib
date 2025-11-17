#ifndef PARSERLIB_LOOP_1_PARSE_NODE_HPP
#define PARSERLIB_LOOP_1_PARSE_NODE_HPP


#include "loop_functions.hpp"


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
            #ifndef NDEBUG
            , m_text("+(" + m_child.text() + ")")
            #endif
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
            return loop_parse(pc, m_child);
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
    loop_1_parse_node<Derived> parse_node<Derived>::operator +() const {
        return *derived();
    }


} //namespace parserlib


#endif //PARSERLIB_LOOP_1_PARSE_NODE_HPP
