#ifndef PARSERLIB_MATCHPARSER_HPP
#define PARSERLIB_MATCHPARSER_HPP


#include <string>
#include "ParserNode.hpp"


namespace parserlib {


    /**
     * A parser that adds a match to the current parse context if another parser parses the input succecssfully.
     * @param ParserNodeType the parser to invoke.
     * @param MatchIdType type of match id.
     */
    template <class ParserNodeType, class MatchIdType> class MatchParser 
        : public ParserNode<MatchParser<ParserNodeType, MatchIdType>> {
    public:
        /**
         * The default constructor.
         * @param child child parser to invoke.
         * @param matchId the match id.
         */
        MatchParser(const ParserNodeType& child, const MatchIdType& matchId) 
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
            return parse(pc, [&]() { return m_child(pc); });
        }

        /**
         * If the child parser succeeds, then a match is added to the context.
         * The object is called to parse within a left recursion parsing context,
         * in order to continue parsing after the non-left recursive part is parsed.
         * @param pc parse context.
         * @param lrc left recursion context.
         * @return true if parsing succeeds, false otherwise.
         */
        template <class ParseContextType> bool parseLeftRecursionContinuation(ParseContextType& pc, LeftRecursionContext<ParseContextType>& lrc) const {
            return parse(pc, [&]() { return m_child.parseLeftRecursionContinuation(pc, lrc); });
        }

    private:
        const ParserNodeType m_child;
        const MatchIdType m_matchId;

        template <class ParseContextType, class PF> bool parse(ParseContextType& pc, const PF& pf) const {
            const auto begin = pc.sourcePosition();
            if (pf()) {
                pc.addMatch(m_matchId, begin, pc.sourcePosition());
                return true;
            }
            return false;
        }

    };


    /**
     * Operator that allows a parse node match to be inserted into the grammar.
     * @param node node to apply the operator to.
     * @param matchId match id.
     * @return a match parser.
     */
    template <class ParserNodeType, class MatchIdType>
    MatchParser<ParserNodeType, MatchIdType>
        operator == (const ParserNode<ParserNodeType>& node, const MatchIdType& matchId) {
        return MatchParser<ParserNodeType, MatchIdType>(static_cast<const ParserNodeType&>(node), matchId);
    }


    /**
     * Operator that allows a parse node match with a character string match id to be inserted into the grammar.
     * @param node node to apply the operator to.
     * @param matchId match id.
     * @return a match parser.
     */
    template <class ParserNodeType, class CharType>
    MatchParser<ParserNodeType, std::basic_string<CharType>>
        operator == (const ParserNode<ParserNodeType>& node, const CharType* matchId) {
        return MatchParser<ParserNodeType, std::basic_string<CharType>>(static_cast<const ParserNodeType&>(node), matchId);
    }


} //namespace parserlib


#endif //PARSERLIB_MATCHPARSER_HPP
