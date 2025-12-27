#ifndef PARSERLIB_SKIP_AFTER_PARSE_NODE_HPP
#define PARSERLIB_SKIP_AFTER_PARSE_NODE_HPP


#include "parse_node.hpp"
#include "parse_algorithms.hpp"


namespace parserlib {


    /**
     * A parse node that can be used to skip input until another parse node parses successfully.
     * Upon return, the parse context state is set to that after the child parse node
     * parses successfully.
     * @param Child Child type.
     */
    template <class Child>
    class skip_after_parse_node : public parse_node<skip_after_parse_node<Child>> {
    public:
        /**
         * The constructor.
         * @param child the child.
         */
        skip_after_parse_node(const Child& child)
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
            if (pc.is_valid_parse_position()) {
                do {
                    bool result = parse_and_restore_state_on_failure(pc, [&]() {
                        return m_child.parse(pc);
                    });
                    if (result) {
                        break;
                    }
                    pc.increment_parse_position();
                } while (pc.is_valid_parse_position());
                return true;
            }
            return false;
        }

    private:
        Child m_child;
    };


    /**
     * Creates a skip after parse node for another parse node.
     * @param val value/parse node the child parse node to debug.
     * @return a debug parse node.
     */
    template <class T>
    auto skip_after(const T& child) {
        return skip_after_parse_node(make_parse_node(child));
    }


} //namespace parserlib


#endif //PARSERLIB_SKIP_AFTER_PARSE_NODE_HPP
