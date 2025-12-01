#ifndef PARSERLIB_MULTIPLE_PARSE_NODE_HPP
#define PARSERLIB_MULTIPLE_PARSE_NODE_HPP


#include <sstream>
#include <type_traits>
#include <cassert>
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
         * @param min_times min times times to use the specific parse node.
         * @param max_times max times times to use the specific parse node.
         * @param child the parse node to use.
         */
        multiple_parse_node(size_t min_times, size_t max_times, const ParseNode& child)
            : m_min_times(min_times)
            , m_max_times(max_times)
            , m_child(child)
        {
            assert(m_min_times <= m_max_times);
        }

        /**
         * Invokes the supplied parse node the supplied amount of times.
         * @param pc the current parse context.
         * @return true on success, false on failure.
         */
        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            const auto initial_state = pc.get_state();
            for (size_t i = 0; i < m_min_times; ++i) {
                if (!m_child.parse(pc)) {
                    pc.set_state(initial_state);
                    return false;
                }
            }
            for (size_t i = m_min_times; i < m_max_times; ++i) {
                if (!m_child.parse(pc)) {
                    pc.set_state(initial_state);
                    break;
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
            stream << "loop(";
            stream << m_min_times << ", ";
            stream << m_max_times << ", ";
            stream << m_child.text();
            stream << ')';
            return stream.str();
        }

        #ifndef NDEBUG
        void init_tree() const override {
            m_child.init_tree();
        }
        #endif

        size_t min_times() const {
            return m_min_times;
        }

        size_t max_times() const {
            return m_max_times;
        }

        const ParseNode& child() const {
            return m_child;
        }

    private:
        const size_t m_min_times;
        const size_t m_max_times;
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
        return multiple_parse_node(times, times, child);
    }


    /**
     * Function that creates a loop from min to max times.
     * @param min_times min times times to use the specific parse node.
     * @param max_times max times times to use the specific parse node.
     * @param child the parse node to use; it must be a parse node object.
     * @return a multiple parse node.
     */
    template <class ParseNode>
    auto loop(size_t min_times, size_t max_times, ParseNode&& child) {
        return multiple_parse_node(min_times, max_times, make_parse_node(child));
    }


    //optimization
    template <class ParseNode>
    auto operator * (size_t times, const multiple_parse_node<ParseNode>& multiple) {
        return multiple_parse_node(times * multiple.min_times(), times * multiple.max_times(), multiple.child());
    }


} //namespace parserlib


#endif //PARSERLIB_MULTIPLE_PARSE_NODE_HPP
