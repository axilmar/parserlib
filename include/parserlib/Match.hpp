#ifndef PARSERLIB_MATCH_HPP
#define PARSERLIB_MATCH_HPP


#include <string>
#include "ParserNode.hpp"


namespace parserlib {


    /**
     * A parser that adds a match to the current parse context if another parser parses the input succecssfully.
     * @param ParserNodeType the parser to invoke.
     * @param MatchIdType type of match id.
     */
    template <class ParserNodeType, class MatchIdType> class Match 
        : public ParserNode<Match<ParserNodeType, MatchIdType>> {
    public:
        /**
         * The default constructor.
         * @param child child parser to invoke.
         * @param matchId the match id.
         */
        Match(const ParserNodeType& child, const MatchIdType& matchId) 
            : m_child(child), m_matchId(matchId) {
        }

        /**
         * Returns the parser to invoke.
         * @return the parser to invoke.
         */
        const ParserNodeType& child() const {
            return m_child;
        }

        /**
         * Returns the match id.
         * @return the match id.
         */
        const MatchIdType& matchId() const {
            return m_matchId;
        }

        /**
         * If the child parser succeeds, then a match is added to the context.
         * @param pc parse context.
         * @return true if the 1st parsing succeeded, false otherwise.
         */
        template <class ParseContextType> bool operator ()(ParseContextType& pc) const {
            const auto begin = pc.sourcePosition();
            if (m_child(pc)) {
                pc.addMatch(m_matchId, begin, pc.sourcePosition());
                return true;
            }
            return false;
        }

    private:
        const ParserNodeType m_child;
        const MatchIdType m_matchId;
    };


    /**
     * Operator that allows a match to be inserted into the grammar.
     * @param node node to apply the operator to.
     * @param matchId match id.
     * @return a match parser.
     */
    template <class ParserNodeType, class MatchIdType>
    Match<ParserNodeType, MatchIdType>
        operator == (const ParserNode<ParserNodeType>& node, const MatchIdType& matchId) {
        return Match<ParserNodeType, MatchIdType>(static_cast<const ParserNodeType&>(node), matchId);
    }


} //namespace parserlib


#endif //PARSERLIB_MATCH_HPP
