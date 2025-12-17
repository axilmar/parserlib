#ifndef PARSERLIB_MATCH_PARSE_NODE_HPP
#define PARSERLIB_MATCH_PARSE_NODE_HPP


#include "parent_parse_node.hpp"


namespace parserlib {


    /**
     * A parse node that adds a match to a parse context when its child node parses successfully.
     * @param MatchId type of match id.
     * @param Child type of parse node to use as a child.
     */
    template <class MatchId, class Child>
    class match_parse_node : public parent_parse_node<match_parse_node<MatchId, Child>, Child> {
    public:
        /** The parent type. */
        using parent_type = parent_parse_node<match_parse_node<MatchId, Child>, Child>;

        /**
         * The constructor.
         * @param id the id to put to the parse context as a match id.
         * @param child the parse node to use as a child.
         */
        match_parse_node(const MatchId& id, const Child& child)
            : parent_type(child)
            , m_id(id)
        {
        }

        /**
         * Invokes the child parse node to parse.
         * If the child parses successfully, then it adds a match
         * to the given parse context.
         * @param pc the parse context to add a match to.
         * @return true on success, false on failure.
         */
        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            pc.save_match_start_state();
            bool result;
            try {
                result = parent_type::get_children().parse(pc);
            }
            catch (...) {
                pc.restore_match_start_state();
                throw;
            }
            pc.restore_match_start_state();
            if (result) {
                pc.add_match(static_cast<int>(m_id));
                return true;
            }
            return false;
        }

    private:
        MatchId m_id;
    };


    /**
     * The operator that is used for creating a match parse node.
     * @param child the child parse node.
     * @param id id of the match.
     * @return a match parse node for the given id and child.
     */
    template <class MatchId, class Child>
    match_parse_node<MatchId, Child> operator ->* (const parse_node<Child>& child, const MatchId& id) {
        return match_parse_node<MatchId, Child>(id, *child.get_impl());
    }


} //namespace parserlib


#endif //PARSERLIB_MATCH_PARSE_NODE_HPP
