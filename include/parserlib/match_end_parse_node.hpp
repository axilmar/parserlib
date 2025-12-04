#ifndef PARSERLIB_MATCH_END_PARSE_NODE_HPP
#define PARSERLIB_MATCH_END_PARSE_NODE_HPP


#include "id_name.hpp"
#include "parse_node.hpp"


namespace parserlib {


    template <class MatchId>
    class match_end_parse_node : public parse_node_base {
    public:
        match_end_parse_node(const MatchId& id)
            : parse_node_base("match_end(" + id_name(id) + ')')
            , m_id(id)
        {
        }

        bool parse(parse_context_interface& pc) const {
            pc.add_match_and_pop_match_start_state(static_cast<int>(m_id));
            return true;
        }

    private:
        const MatchId m_id;
    };


    template <class MatchId>
    match_end_parse_node<MatchId> match_end(const MatchId& id) {
        return id;
    }


} //namespace parserlib


#endif //PARSERLIB_MATCH_END_PARSE_NODE_HPP
