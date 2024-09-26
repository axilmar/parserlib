#ifndef PARSERLIB_MATCH_HPP
#define PARSERLIB_MATCH_HPP


#include <vector>
#include <ostream>
#include "SourceString.hpp"


namespace parserlib {


    template <class MatchId = int, class Source = SourceString<>>
    class Match;


    template <class MatchId, class Source>
    using MatchContainer = std::vector<Match<MatchId, Source>>;


    template <class MatchId, class Source>
    class Match {
    public:
        typedef Source Source;
        typedef typename Source::const_iterator Iterator;
        typedef MatchContainer<MatchId, Source> MatchContainer;

        Match() 
            : m_matchId()
        {
        }

        Match(const MatchId& matchId, const Iterator& startPosition, const Iterator& endPosition, MatchContainer&& children)
            : m_matchId(matchId)
            , m_startPosition(startPosition)
            , m_endPosition(endPosition)
            , m_children(std::move(children))
        {
        }

        const MatchId& getMatchId() const {
            return m_matchId;
        }

        const Iterator& getStartPosition() const {
            return m_startPosition;
        }

        const Iterator& getEndPosition() const {
            return m_startPosition;
        }

        const MatchContainer& getChildren() const {
            return m_children;
        }

        Source getSource() const {
            return Source(m_startPosition, m_endPosition);
        }

        template <class Elem, class Traits>
        void print(std::basic_ostream<Elem, Traits>& stream, size_t depth = 0, size_t tabSize = 4, size_t sourceMaxCharsPerLine = 16) const {
            for (size_t i = 0; i < depth * tabSize; ++i) {
                stream << ' ';
            }
            stream << m_matchId << ": " << getSource(sourceMaxCharsPerLine) << std::endl;
            for (const auto& child : m_children) {
                child.print(stream, depth + 1, tabSize, sourceMaxCharsPerLine);
            }
        }

    private:
        MatchId m_matchId;
        Iterator m_startPosition;
        Iterator m_endPosition;
        MatchContainer m_children;

        Source getSource(size_t sourceMaxCharsPerLine) const {
            size_t size = m_endPosition - m_startPosition;

            if (size <= sourceMaxCharsPerLine) {
                return Source(m_startPosition, m_endPosition);
            }

            return Source(m_startPosition, m_startPosition + sourceMaxCharsPerLine);
        }
    };


} //namespace parserlib


#endif //PARSERLIB_MATCH_HPP
