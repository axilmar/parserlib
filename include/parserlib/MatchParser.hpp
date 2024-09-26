#ifndef PARSERLIB_MATCHPARSER_HPP
#define PARSERLIB_MATCHPARSER_HPP


#include "Parser.hpp"


namespace parserlib {


    template <class Child, class MatchId>
    class MatchParser : public Parser<MatchParser<Child, MatchId>> {
    public:
        MatchParser(const Child& child, const MatchId& id)
            : m_child(child)
            , m_id(id)
        {
        }

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
