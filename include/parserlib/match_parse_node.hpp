#ifndef PARSERLIB_MATCH_PARSE_NODE_HPP
#define PARSERLIB_MATCH_PARSE_NODE_HPP


#include "parse_node.hpp"


namespace parserlib {


    /**
     * A parse node that adds a match to a parse context
     * when another node parses successfully.
     * @param MatchId type of id of the parse node.
     * @param Child child type.
     */
    template <class MatchId, class Child>
    class match_parse_node : public parse_node<match_parse_node<MatchId, Child>> {
    public:
        /**
         * The constructor.
         * @param id the match id.
         * @param child the child.
         */
        match_parse_node(const MatchId& id, const Child& child)
            : m_id(id)
            , m_child(child)
        {
        }

        /**
         * If the child parses successfully,
         * then it adds a match to the parse context.
         * @param pc the parse context.
         * @return true on success, false on failure.
         */
        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            const auto mps = pc.get_match_parse_state();
            if (m_child.parse(pc)) {
                pc.add_match(m_id, mps, pc.get_iterator());
                return true;
            }
            return false;
        }

    private:
        MatchId m_id;
        Child m_child;
    };


    /**
     * Operator that creates a match parse node.
     * @param pn parse node to create a match for.
     * @param id match id.
     * @return a match parse node.
     */
    template <class Impl, class MatchId>
    auto operator ->* (const parse_node<Impl>& pn, const MatchId& id) {
        return match_parse_node<MatchId, Impl>(id, *pn.get_impl());
    }


} //namespace parserlib


#endif //PARSERLIB_MATCH_PARSE_NODE_HPP
