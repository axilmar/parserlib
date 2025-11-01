#ifndef PARSERLIB_MATCH_PARSE_NODE_HPP
#define PARSERLIB_MATCH_PARSE_NODE_HPP


#include "parse_node.hpp"


namespace parserlib {


    /**
     * A parse node that adds a match to a parse context,
     * if the match's child node parses successfully.
     * @param MatchId type of the match id.
     * @param ParseNode type of child.
     */
    template <class MatchId, class ParseNode>
    class match_parse_node : public parse_node<match_parse_node<MatchId, ParseNode>> {
    public:
        /**
         * The constructor.
         * @param id the match id.
         * @param child the child node to use for parsing.
         */
        match_parse_node(const MatchId& id, const ParseNode& child) 
            : m_id(id), m_child(child)
        {
        }

        /**
         * Parses the child node.
         * If the child node parses succesfully, then a match is added to the given parse context.
         * @param the current parse context.
         * @return true if parsing succeeds, false otherwise.
         */
        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            const auto match_start_state = pc.get_match_start_state();
            if (m_child.parse(pc)) {
                pc.add_match(m_id, match_start_state, pc.iterator());
                return true;
            }
            return false;
        }

        const MatchId& id() const {
            return m_id;
        }

        const ParseNode& child() const {
            return m_child;
        }

    private:
        const MatchId m_id;
        const ParseNode m_child;
    };


    /**
     * Operator that allows the creation of a match from a parse node.
     * @param pn the parse node to use as a child of the match parse node.
     * @param id the match id to store to the parse context if parsing succeeds.
     * @return a match parse node.
     */
    template <class MatchId, class ParseNode>
    auto operator ->* (ParseNode&& pn, const MatchId& id) {
        return match_parse_node(id, make_parse_node(std::forward<ParseNode>(pn)));
    }


} //namespace parserlib


#endif //PARSERLIB_MATCH_PARSE_NODE_HPP
