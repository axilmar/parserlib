#ifndef PARSERLIB_NEWLINE_PARSE_NODE_HPP
#define PARSERLIB_NEWLINE_PARSE_NODE_HPP


#include "parse_node.hpp"


namespace parserlib {


    /**
     * A parse node that can be used to increment a parse context's current line,
     * when another parser parses successfully.
     * @param Child Type of the parse node to newline.
     */
    template <class Child>
    class newline_parse_node : public parse_node<newline_parse_node<Child>> {
    public:
        /**
         * The constructor.
         * @param child the child.
         */
        newline_parse_node(const Child& child)
            : m_child(child)
        {
        }

        /**
         * It invokes the child node to parse.
         * If the child node parses successfully, then the parse position line is incremented.
         * @param pc the context to pass to the child.
         * @return the result of the child parse node.
         */
        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            if (m_child.parse(pc)) {
                pc.increment_parse_position_line();
                return true;
            }
            return false;
        }

    private:
        Child m_child;
    };


    /**
     * Creates a newline parse node for another parse node.
     * @param value value or parse node to create a newline parser for.
     * @return a newline parse node.
     */
    template <class T>
    auto newline(const T& value) {
        return newline_parse_node(make_parse_node(value));
    }


    /**
     * Creates a newline parse node for the character '\n'.
     * @return a newline parse node for the character '\n'.
     */
    inline auto newline() {
        return newline('\n');
    }


} //namespace parserlib


#endif //PARSERLIB_NEWLINE_PARSE_NODE_HPP
