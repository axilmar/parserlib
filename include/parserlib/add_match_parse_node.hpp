#ifndef PARSERLIB_ADD_MATCH_PARSE_NODE_HPP
#define PARSERLIB_ADD_MATCH_PARSE_NODE_HPP


#include "id_name.hpp"
#include "parse_node.hpp"


namespace parserlib {


    template <class MatchId>
    class add_match_parse_node : public parse_node_base {
    public:
        add_match_parse_node(const MatchId& id)
            : m_id(id)
        {
        }

        bool parse(parse_context_interface& pc) const {
            pc.add_match(static_cast<int>(m_id));
            return true;
        }

        const MatchId& id() const {
            return m_id;
        }

    private:
        const MatchId m_id;
    };


    template <class MatchId>
    add_match_parse_node<MatchId> match(const MatchId& id) {
        return id;
    }


} //namespace parserlib


#endif //PARSERLIB_ADD_MATCH_PARSE_NODE_HPP
