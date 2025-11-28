#ifndef PARSERLIB_MEMOIZED_PARSE_NODE_HPP
#define PARSERLIB_MEMOIZED_PARSE_NODE_HPP


#include "parse_node.hpp"


namespace parserlib {


    /**
     * A parse node that uses the memoization feature of a parse context
     * to parse an expression.
     * @param ParseNode type parse node to memoize its result of.
     */
    template <class ParseNode>
    class memoized_parse_node : public parse_node<memoized_parse_node<ParseNode>> {
    public:
        /**
         * The constructor.
         * @param parse_node the parse node to memoize its result of.
         */
        memoized_parse_node(const ParseNode& parse_node) 
            : m_parse_node(parse_node)
            , m_id(id())
        {
        }

        /**
         * Invokes the `parse_memoized` method of the given parse context with the supplied parse node.
         * @param pc the current parse context.
         * @return true on success, false on failure.
         */
        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            return pc.parse_memoized(m_parse_node, m_id);
        }

        /**
         * Interface for returning the text description of the node.
         * @return the text description of the node.
         */
        std::string text() const override {
            return "memoized(" + m_parse_node.text() + ')';
        }

        #ifndef NDEBUG
        void init_tree() const override {
            m_parse_node.init();
        }
        #endif

    private:
        const ParseNode m_parse_node;
        const int m_id;

        static int id() {
            static thread_local int id = 0;
            return ++id;
        }
    };


    /**
     * Function to create a memoized parse node.
     * @param parse_node the parse node to memoize.
     * @return a memoized parse node.
     */
    template <class ParseNode>
    auto memoized(ParseNode&& parse_node) {
        return memoized_parse_node(make_parse_node(parse_node));
    }


} //namespace parserlib


#endif //PARSERLIB_MEMOIZED_PARSE_NODE_HPP
