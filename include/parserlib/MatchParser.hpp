#ifndef PARSERLIB_MATCHPARSER_HPP
#define PARSERLIB_MATCHPARSER_HPP


#include "Parser.hpp"


namespace parserlib {


    /**
     * Parser used to add a match when the child parser parses successfully.
     * 
     * @param Child the child parser.
     * @param MatchId the match id type.
     */
    template <class Child, class MatchId>
    class MatchParser : public Parser<MatchParser<Child, MatchId>> {
    public:
        /**
         * The constructor.
         * @param child the child parser.
         * @param id the match id.
         */
        MatchParser(const Child& child, const MatchId& id)
            : m_child(child)
            , m_id(id)
        {
        }

        /**
         * Parses the input.
         * 
         * It invokes the child parser.
         * 
         * If the child parser parses successfully, then a new match is added to the parse context.
         * 
         * The children of this match are all the matches created within this call.
         * 
         * @param pc the parse context.
         * @return whatever the child parser returns.
         */
        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            auto [startPosition, startMatchIndex] = pc.getMatchStart();
            if (m_child.parse(pc)) {
                auto [endPosition, endMatchIndex] = pc.getMatchEnd();
                pc.addMatch(m_id, startPosition, endPosition, endMatchIndex - startMatchIndex);
                return true;
            }
            return false;
        }

    private:
        Child m_child;
        MatchId m_id;
    };


} //namespace parserlib


#endif //PARSERLIB_MATCHPARSER_HPP
