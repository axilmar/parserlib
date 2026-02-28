#ifndef PARSERLIB_MATCH_PARSE_NODE_HPP
#define PARSERLIB_MATCH_PARSE_NODE_HPP


#include "parse_node_ptr.hpp"


namespace parserlib {


    template <class ParseContext>
    class match_parse_node : public parse_node<ParseContext> {
    public:
        using id_type = typename ParseContext::match_id_type;

        match_parse_node(const parse_node_ptr<ParseContext>& parse_node, const id_type& id)
            : m_parse_node(parse_node)
            , m_id(id)
        {
        }

        bool parse(ParseContext& pc) const override {
            const auto from_state = pc.get_match_parse_state();
            if (m_parse_node->parse(pc)) {
                pc.add_match(m_id, from_state);
                return true;
            }
            return false;
        }

    private:
        parse_node_ptr<ParseContext> m_parse_node;
        id_type m_id;
    };


    template <class ParseContext>
    parse_node_ptr<ParseContext> operator ->* (const parse_node_ptr<ParseContext>& parse_node, const typename ParseContext::match_id_type& id) {
        return std::make_shared<match_parse_node<ParseContext>>(parse_node, id);
    }


} //namespace parserlib


#endif //PARSERLIB_MATCH_PARSE_NODE_HPP
