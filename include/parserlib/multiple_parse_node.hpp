#ifndef PARSERLIB_MULTIPLE_PARSE_NODE_HPP
#define PARSERLIB_MULTIPLE_PARSE_NODE_HPP


#include <sstream>
#include <type_traits>
#include "parse_node.hpp"


namespace parserlib {


    /**
     * A parse node that uses another parse node a specific amount of times
     * to parse.
     * @param ParseNode type of parse node to use.
     */
    template <class ParseNode>
    class multiple_parse_node : public parse_node<multiple_parse_node<ParseNode>> {
    public:
        /**
         * The constructor.
         * @param times times to use the specific parse node.
         * @param child the parse node to use.
         */
        multiple_parse_node(size_t times, const ParseNode& child)
            : m_times(times)
            , m_child(child)
        {
        }

        /**
         * Invokes the supplied parse node the supplied amount of times.
         * If there is an error, then the whole parsing is cancelled,
         * since this parse node is a sequence parse node.
         * @param pc the current parse context.
         * @return true on success, false on failure.
         */
        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            const auto initial_state = pc.get_state();
            for (size_t i = 0; i < m_times; ++i) {
                if (!m_child.parse(pc)) {
                    pc.set_state(initial_state);
                    return false;
                }
            }
            return true;
        }

        /**
         * Converts the parse node to a textual description.
         * @return a string of this parse node as text.
         */
        std::string text() const override {
            std::stringstream stream;
            stream << '(';
            stream << m_times;
            stream << " * ";
            stream << m_child.text();
            stream << ')';
            return stream.str();
        }

        #ifndef NDEBUG
        void init_tree() const override {
            m_child.init_tree();
        }
        #endif

        size_t times() const {
            return m_times;
        }

        const ParseNode& child() const {
            return m_child;
        }

    private:
        const size_t m_times;
        const ParseNode m_child;
    };


    /**
     * Creates a multiple parse node from a times value and a parse node.
     * @param times times to repeat the given parse node.
     * @param child the parse node to use; it must be a parse node object.
     * @return a multiple parse node.
     */
    template <class ParseNode, std::enable_if_t<std::is_base_of_v<parse_node_base, ParseNode>, bool> = true>
    auto operator * (size_t times, const ParseNode& child) {
        return multiple_parse_node(times, child);
    }


    //optimization
    template <class ParseNode>
    auto operator * (size_t times, const multiple_parse_node<ParseNode>& multiple) {
        return multiple_parse_node(times * multiple.times(), multiple.child());
    }


} //namespace parserlib


#endif //PARSERLIB_MULTIPLE_PARSE_NODE_HPP
