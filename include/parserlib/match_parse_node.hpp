#ifndef PARSERLIB_MATCH_PARSE_NODE_HPP
#define PARSERLIB_MATCH_PARSE_NODE_HPP


#include <type_traits>
#include "unary_parse_node.hpp"


namespace parserlib {


    template <class T, class MatchId>
    class match_parse_node : public unary_parse_node<match_parse_node<T, MatchId>, T> {
    public:
        match_parse_node(const T& child, const MatchId& id)
            : unary_parse_node<match_parse_node<T, MatchId>, T>(child)
            , m_id(id)
        {
        }

        bool parse(parse_context_interface& pc) const {
            pc.push_match_start_state();
            if (this->child().parse(pc)) {
                pc.add_match(static_cast<int>(m_id));
                pc.pop_match_start_state();
                return true;
            }
            pc.pop_match_start_state();
            return false;
        }

        const MatchId& id() const {
            return m_id;
        }

    private:
        const MatchId m_id;
    };


    template <class T, class MatchId, std::enable_if_t<std::is_base_of_v<parse_node_tag, T>, bool> = true>
    auto operator ->* (const T& child, const MatchId& id) {
        return match_parse_node<T, MatchId>(child, id);
    }


} //namespace parserlib


#endif //PARSERLIB_MATCH_PARSE_NODE_HPP
