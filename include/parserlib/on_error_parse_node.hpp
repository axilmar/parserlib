#ifndef PARSERLIB_ON_ERROR_PARSE_NODE_HPP
#define PARSERLIB_ON_ERROR_PARSE_NODE_HPP


#include "parse_node.hpp"
#include "skip_node_base.hpp"


namespace parserlib {


    /**
     * A parse node which skips an input, when another parse node either fails to parse
     * or has created errors.
     * @param ParseNode type of parse node to use for parsing.
     * @param SkipParseNode type of parse node to use for skipping the erroneous input.
     */
    template <class ParseNode, class SkipParseNode> 
    class on_error_parse_node : public parse_node<on_error_parse_node<ParseNode, SkipParseNode>> {
    public:
        /**
         * The constructor.
         * @param parse_node node to use for parsing.
         * @param skip_parse_node node to use for skipping the erroneous input.
         */
        on_error_parse_node(const ParseNode& parse_node, const SkipParseNode& skip_parse_node)
            : m_parse_node(parse_node)
            , m_skip_parse_node(skip_parse_node)
        {
        }

        /**
         * If the supplied parse node returns false or errors are increased,
         * then it invokes the parse node to skip the input.
         * @param pc the current parse context.
         * @return true if the parse was successful or successfully skipped the erroneous input.
         */
        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            const size_t error_count_before_parse = pc.errors().size();
            if (!pc.parse(m_parse_node)) {
                return pc.parse(m_skip_parse_node);
            }
            if (pc.errors().size() > error_count_before_parse) {
                pc.set_parse_position_after_last_error();
                return pc.parse(m_skip_parse_node);
            }
            return true;
        }

        /**
         * Converts the parse node to a textual description.
         * @return a string of this parse node as text.
         */
        std::string text() const override {
            std::stringstream stream;
            stream << "on_error(" << m_parse_node.text() << ", " << m_skip_parse_node.text() << ")";
            return stream.str();
        }

        #ifndef NDEBUG
        void init_tree() const override {
            m_skip_parse_node.init();
        }
        #endif

    private:
        const ParseNode m_parse_node;
        const SkipParseNode m_skip_parse_node;
    };


    /**
     * Creates a parse node which skips the input when an error happens.
     * @param parse_node the parse node to use for parsing.
     * @param skip_parse_node parse node to use for skipping the erroneous input.
     * @return an error parse node.
     */
    template <class ParseNode, class SkipParseNode, std::enable_if_t<std::is_base_of_v<skip_node_base, std::decay_t<SkipParseNode>>, bool> = true>
    auto on_error(ParseNode&& parse_node, SkipParseNode&& skip_parse_node) {
        return on_error_parse_node(make_parse_node(parse_node), make_parse_node(skip_parse_node));
    }


} //namespace parserlib


#endif //PARSERLIB_ON_ERROR_PARSE_NODE_HPP
