#ifndef PARSERLIB_MATCH_PARSE_NODE_HPP
#define PARSERLIB_MATCH_PARSE_NODE_HPP


#include "parse_node.hpp"


namespace parserlib {


    template <class MatchId>
    class match_parse_node : public interface::parse_node {
    public:
        match_parse_node(const MatchId& id, const parserlib::parse_node& child) : m_id(id), m_child(child) {
        }

        bool parse(interface::parse_context& pc) const override {
            pc.push_match_start_state();
            try {
                if (m_child.parse(pc)) {
                    pc.add_match(static_cast<int>(m_id));
                    pc.pop_match_start_state();
                    return true;
                }
                pc.pop_match_start_state();
                return false;
            }
            catch (...) {
                pc.pop_match_start_state();
                throw;
            }
        }

    private:
        const MatchId m_id;
        const parserlib::parse_node m_child;
    };


    template <class MatchId>
    parse_node operator ->* (const parse_node& pn, const MatchId& id) {
        return interface::create_parse_node<match_parse_node<MatchId>>(id, pn);
    }


} //namespace parserlib


#endif //PARSERLIB_MATCH_PARSE_NODE_HPP
