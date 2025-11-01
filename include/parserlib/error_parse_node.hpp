#ifndef PARSERLIB_ERROR_PARSE_NODE_HPP
#define PARSERLIB_ERROR_PARSE_NODE_HPP


#include "parse_node.hpp"


namespace parserlib {


    /**
     * An error parse node.
     * @param ErrorId type of error id.
     * @param SkipParseNode parse node to use for skipping the erroneous input.
     */
    template <class ErrorId, class SkipParseNode> 
    class error_parse_node : public parse_node<error_parse_node<ErrorId, SkipParseNode>> {
    public:
        /**
         * The constructor.
         * @param id id of error.
         * @param skip_parse_node node to use for skipping the erroneous input.
         */
        error_parse_node(const ErrorId& id, const SkipParseNode& skip_parse_node)
            : m_id(id)
            , m_skip_parse_node(skip_parse_node)
        {
        }

        /**
         * Skips the error input by invoking the skip parse node in a loop, 
         * incrementing the parse position at each iteration,
         * until the skip parse node returns true or the input ends.
         * @param pc the current parse context.
         * @return always true, in order to avoid backtracking (which will cancel the error if it happens).
         */
        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            const auto start_pos = pc.parse_position();
            while (pc.parse_valid()) {
                const auto state = pc.get_state();
                const bool result = m_skip_parse_node.parse(pc);
                pc.set_state(state);
                if (result) {
                    break;
                }
                pc.increment_parse_position();
            }
            pc.add_error(m_id, start_pos, pc.iterator());
            return true;
        }

    private:
        const ErrorId m_id;
        const SkipParseNode m_skip_parse_node;
    };


    /**
     * Creates an error parse node.
     * @param id error id.
     * @param skip_parse_node parse node to use for skipping the erroneous input.
     * @return an error parse node.
     */
    template <class ErrorId, class SkipParseNode>
    auto error(const ErrorId& id, SkipParseNode&& skip_parse_node) {
        return error_parse_node(id, make_parse_node(skip_parse_node));
    }


} //namespace parserlib


#endif //PARSERLIB_ERROR_PARSE_NODE_HPP
