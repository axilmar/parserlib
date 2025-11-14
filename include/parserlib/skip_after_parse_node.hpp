#ifndef PARSERLIB_SKIP_AFTER_PARSE_NODE_HPP
#define PARSERLIB_SKIP_AFTER_PARSE_NODE_HPP


#include "parse_node.hpp"


namespace parserlib {


    /**
     * A parse node that allows skipping input by calling a parser in a loop,
     * until the parser parses successfully or the input is exhausted.
     * The parsing state after the parse is the parsing state after executing the parser
     * for the final time.
     * @param ParseNode the parse node to use for parsing.
     */
    template <class ParseNode>
    class skip_after_parse_node : public parse_node<skip_after_parse_node<ParseNode>> {
    public:
        /**
         * The constructor.
         * @param parse_node the parse node to use.
         */
        skip_after_parse_node(const ParseNode& parse_node)
            : m_parse_node(parse_node)
        {
        }

        /**
         * It invokes the supplied parser in a loop.
         * It stops when the parser parses sucessfully
         * or the input is exhausted.
         * After the call, the parse state is the one after the last call of the parser.
         * @param pc the parse context to use.
         * @return true if the input was skipped successfully, false if the input is exhausted.
         */
        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            const auto initial_state = pc.get_state();
            while (pc.parse_valid()) {
                const auto state = pc.get_state();
                try {
                    const bool result = m_parse_node.parse(pc);
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
            return false;
        }

    private:
        const ParseNode m_parse_node;
    };


    /**
     * Function that creates a skip_after_parse_node instance.
     * @param parse_node the parse node to use for parsing.
     * @return the appropriate skip parse node.
     */
    template <class ParseNode>
    auto skip_after(ParseNode&& parse_node) {
        return skip_after_parse_node(make_parse_node(parse_node));
    }


} //namespace parserlib


#endif //PARSERLIB_SKIP_AFTER_PARSE_NODE_HPP
