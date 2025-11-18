#ifndef PARSERLIB_ERROR_MATCH_PARSE_NODE_HPP
#define PARSERLIB_ERROR_MATCH_PARSE_NODE_HPP


#include <sstream>
#include "parse_node.hpp"


namespace parserlib {


    /**
     * A parse node that adds a match into a parse context, in case of error.
     * It is used in cases where a failure to parse must not break the match tree structure.
     * @param MatchId type of match id.
     */
    template <class MatchId>
    class error_match_parse_node : public parse_node<error_match_parse_node<MatchId>> {
    public:
        /**
         * The constructor.
         * @param id the match id.
         */
        error_match_parse_node(const MatchId& id) 
            : m_id(id)
        {
        }

        /**
         * Adds a match with the supplied id at the current parse position.
         * @param pc the current parse context.
         * @return always true.
         */
        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            pc.add_match(m_id, pc.get_state(), pc.iterator());
            return true;
        }

        /**
         * Converts the parse node to a textual description.
         * @return a string of this parse node as text.
         */
        std::string text() const override {
            std::stringstream stream;
            stream << "error_match(" << id_name<MatchId>::get(m_id) << ")";
            return stream.str();
        }

    private:
        const MatchId m_id;
    };


    /**
     * Creates a parse node that adds a match into a parse context, in case of error.
     * It is used in cases where a failure to parse must not break the match tree structure.
     * @param id the match id.
     * @return the error match parse node.
     */
    template <class MatchId>
    error_match_parse_node<MatchId> error_match(const MatchId& id) {
        return { id };
    }


} //namespace parserlib


#endif //PARSERLIB_ERROR_MATCH_PARSE_NODE_HPP
