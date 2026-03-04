#ifndef PARSERLIB_ERROR_PARSE_NODE_HPP
#define PARSERLIB_ERROR_PARSE_NODE_HPP


#include "parse_node_ptr.hpp"


namespace parserlib {


    template <class ParseContext>
    class error_parse_node : public parse_node<ParseContext> {
    public:
        using id_type = typename ParseContext::error_id_type;

        error_parse_node(const parse_node_ptr<ParseContext>& parse_node, const id_type& id)
            : m_parse_node(parse_node)
            , m_id(id)
        {
        }

        bool parse(ParseContext& pc) const override {
            const auto from_iterator = pc.get_iterator();
            if (m_parse_node->parse(pc)) {
                pc.add_error(m_id, from_iterator);
                return true;
            }
            return false;
        }

    private:
        parse_node_ptr<ParseContext> m_parse_node;
        id_type m_id;
    };


} //namespace parserlib


#endif //PARSERLIB_ERROR_PARSE_NODE_HPP
