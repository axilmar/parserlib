#ifndef PARSERLIB_ERROR_PARSE_NODE_HPP
#define PARSERLIB_ERROR_PARSE_NODE_HPP


#include <type_traits>
#include "bool_parse_node.hpp"
#include "skip_node_base.hpp"


namespace parserlib {


    /**
     * An error parse node.
     * It adds an error to the given parse context.
     * @param ErrorId type of error id.
     * @param SkipParseNode Type of parse node to use for skipping the erroneous input.
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
            if (pc.parse_valid()) {
                const auto start_pos = pc.parse_position();
                if (pc.parse(m_skip_parse_node)) {
                    pc.add_error(m_id, start_pos, pc.iterator());
                    return true;
                }
            }
            return false;
        }

        /**
         * Converts the parse node to a textual description.
         * @return a string of this parse node as text.
         */
        std::string text() const override {
            std::stringstream stream;
            stream << "error(" << id_name<ErrorId>::get(m_id) << ", " << m_skip_parse_node.text() << ")";
            return stream.str();
        }

        #ifndef NDEBUG
        void init_tree() const override {
            m_skip_parse_node.init();
        }
        #endif

    private:
        const ErrorId m_id;
        const SkipParseNode m_skip_parse_node;
    };


    /**
     * Creates an error parse node with a specific skip parse node.
     * @param id error id.
     * @param skip_parse_node parse node to use for skipping the erroneous input.
     * @return an error parse node.
     */
    template <class ErrorId, class SkipParseNode, std::enable_if_t<std::is_base_of_v<skip_node_base, std::decay_t<SkipParseNode>>, bool> = true>
    auto error(const ErrorId& id, SkipParseNode&& skip_parse_node) {
        return error_parse_node(id, make_parse_node(skip_parse_node));
    }


    /**
     * Creates an error parse node with no parse node; after the error, the parse 
     * context remains as is.
     * @param id error id.
     * @return an error parse node.
     */
    template <class ErrorId>
    auto error(const ErrorId& id) {
        return error_parse_node(id, make_parse_node(true));
    }


} //namespace parserlib


#endif //PARSERLIB_ERROR_PARSE_NODE_HPP
