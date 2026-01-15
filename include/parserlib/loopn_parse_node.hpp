#ifndef PARSERLIB_LOOPN_PARSE_NODE_HPP
#define PARSERLIB_LOOPN_PARSE_NODE_HPP


#include <type_traits>
#include "parse_node.hpp"


namespace parserlib {


    /**
     * A parse node that makes another parse node be repeated a specific amount of times.
     * @param Child the type of the child parse node.
     */
    template <class Child>
    class loopn_parse_node : public parse_node<loopn_parse_node<Child>> {
    public:
        /**
         * The constructor.
         * @param child the child parse node.
         * @@param times times to repeat the given child.
         */
        loopn_parse_node(const Child& child, size_t times)
            : m_child(child)
            , m_times(times)
        {
        }

        /**
         * Invokes the child once and if the child succeeds,
         * then it invokes the child in a 'while' loop,
         * until the child cannot parse any more.
         * @param pc the parse context.
         * @return always true.
         */
        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            const auto state = pc.get_state();
            for (size_t i = 0; i < m_times; ++i) {
                try {
                    if (!m_child.parse(pc)) {
                        pc.set_state(state);
                        return false;
                    }
                }
                catch (...) {
                    pc.set_state(state);
                    throw;
                }
            }
            return true;
        }

    private:
        Child m_child;
        size_t m_times;
    };


    /**
     * Creates a parse node that repeats another parse node a specific amount of times.
     * @param parse_node the parse node to repeat.
     * @param times the repetition count.
     * @return a loop parse node object.
     */
    template <class ParseNode, std::enable_if_t<std::is_base_of_v<parse_node_tag, ParseNode>, bool> = true>
    auto operator *(const ParseNode& parse_node, size_t times) {
        return loopn_parse_node(parse_node, times);
    }


} //namespace parserlib


#endif //PARSERLIB_LOOPN_PARSE_NODE_HPP
