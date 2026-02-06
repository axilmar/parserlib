#ifndef PARSERLIB_SKIP_BEFORE_PARSE_NODE_HPP
#define PARSERLIB_SKIP_BEFORE_PARSE_NODE_HPP


#include "parse_node.hpp"
#include "parse_algorithms.hpp"


namespace parserlib {


    /**
     * A parse node that can be used to skip input until another parse node parses successfully.
     * Upon return, the parse context state is set to that before the child parse node
     * parses successfully.
     * @param Child Child type.
     */
    template <class Child>
    class skip_before_parse_node : public parse_node<skip_before_parse_node<Child>> {
    public:
        /**
         * The constructor.
         * @param child the child.
         */
        skip_before_parse_node(const Child& child)
            : m_child(child)
        {
        }

        /**
         * It invokes the child node to parse.
         * A breakpoint can be placed here for debugging.
         * @param pc the context to pass to the child.
         * @return always true.
         */
        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            for(;;) {
                bool result = parse_and_restore_state(pc, [&]() {
                    return m_child.parse(pc);
                });
                if (result) {
                    return result;
                }
                if (pc.is_end_parse_position()) {
                    break;
                }
                pc.increment_parse_position();
            }
            return false;
        }

    private:
        Child m_child;
    };


    /**
     * Creates a skip before parse node for another parse node.
     * @param val value/parse node the child parse node to use for testing.
     * @return a skip parse node.
     */
    template <class T>
    auto skip_before(const T& child) {
        return skip_before_parse_node(make_parse_node(child));
    }


} //namespace parserlib


#endif //PARSERLIB_SKIP_BEFORE_PARSE_NODE_HPP
