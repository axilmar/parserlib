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
         * Invokes the skip parse node to skip the erroneous input, 
         * and if the skip parse node returns true,
         * then adds an error to the parse context.
         * @param pc the current parse context.
         * @return true if the skip parse node returned true, false otherwise.
         */
        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            const auto start_pos = pc.parse_position();
            if (m_skip_parse_node.parse(pc)) {
                pc.add_error(m_id, start_pos, pc.iterator());
                return true;
            }
            return false;
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
