#ifndef PARSERLIB_SKIP_BEFORE_PARSE_NODE_HPP
#define PARSERLIB_SKIP_BEFORE_PARSE_NODE_HPP


#include "parse_node.hpp"
#include "skip_node_base.hpp"


namespace parserlib {


    /**
     * A parse node that allows skipping input by calling a parser in a loop,
     * until the parser parses successfully or the input is exhausted.
     * The parsing state after the parse is the parsing state before executing the parser
     * for the final time.
     * @param ParseNode the parse node to use for parsing.
     */
    template <class ParseNode>
    class skip_before_parse_node : public parse_node<skip_before_parse_node<ParseNode>>, public skip_node_base {
    public:
        /**
         * The constructor.
         * @param parse_node the parse node to use.
         */
        skip_before_parse_node(const ParseNode& parse_node)
            : m_parse_node(parse_node)
        {
        }

        /**
         * It invokes the supplied parser in a loop.
         * It stops when the parser parses sucessfully
         * or the input is exhausted.
         * After the call, the parse state is the one before the last call of the parser.
         * @param pc the parse context to use.
         * @return always true.
         */
        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            const auto initial_state = pc.get_state();
            while (pc.parse_valid()) {
                const auto state = pc.get_state();
                try {
                    const bool result = pc.parse(m_parse_node);
                    pc.set_state(state);
                    if (result) {
                        return true;
                    }
                }
                catch (...) {
                    pc.set_state(initial_state);
                    throw;
                }
                pc.set_state(state);
                pc.increment_parse_position();
            }
            return true;
        }

        /**
         * Converts the parse node to a textual description.
         * @return a string of this parse node as text.
         */
        std::string text() const override {
            return "skip_before(" + m_parse_node.text() + ")";
        }

        #ifndef NDEBUG
        void init_tree() const override {
            m_parse_node.init();
        }
        #endif

    private:
        const ParseNode m_parse_node;
    };


    /**
     * Function that creates a skip_before_parse_node instance.
     * @param parse_node the parse node to use for parsing.
     * @return the appropriate skip parse node.
     */
    template <class ParseNode>
    auto skip_before(ParseNode&& parse_node) {
        return skip_before_parse_node(make_parse_node(parse_node));
    }


} //namespace parserlib


#endif //PARSERLIB_SKIP_BEFORE_PARSE_NODE_HPP
