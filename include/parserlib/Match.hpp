#ifndef PARSERLIB_MATCH_HPP
#define PARSERLIB_MATCH_HPP


#include <vector>
#include <ostream>
#include "SourceString.hpp"


namespace parserlib {


    template <class MatchId = int, class Source = SourceString<>>
    class Match;


    /**
     * Type of match container.
     * @param MatchId match id type.
     * @param Source source type.
     */
    template <class MatchId, class Source>
    using MatchContainer = std::vector<Match<MatchId, Source>>;


    /**
     * Type of match.
     * @param MatchId match id type.
     * @param Source source type.
     */
    template <class MatchId, class Source>
    class Match {
    public:
        /**
         * The match id type.
         */
        typedef MatchId MatchId;

        /**
         * The source type.
         */
        typedef Source Source;

        /**
         * The source iterator type.
         */
        typedef typename Source::const_iterator Iterator;

        /**
         * The match container type.
         */
        typedef MatchContainer<MatchId, Source> MatchContainer;

        /**
         * The default constructor.
         * An empty match is created.
         */
        Match() 
            : m_matchId()
        {
        }

        /**
         * Constructs a match from parameters.
         * @param matchId match id.
         * @param startPosition start position into the source.
         * @param endPosition end position into the source (non-inclusive, one position past the last element).
         * @param children children matches.
         */
        Match(const MatchId& matchId, const Iterator& startPosition, const Iterator& endPosition, MatchContainer&& children)
            : m_matchId(matchId)
            , m_startPosition(startPosition)
            , m_endPosition(endPosition)
            , m_children(std::move(children))
        {
        }

        /**
         * Returns the match id.
         * @return the match id.
         */
        const MatchId& getId() const {
            return m_matchId;
        }

        /**
         * Returns the start position of the match into the source.
         * @return the start position of the match into the source.
         */
        const Iterator& getStartPosition() const {
            return m_startPosition;
        }

        /**
         * Returns the end position of the match into the source.
         * The end position is one position after the last matched element.
         * @return the end position of the match into the source.
         */
        const Iterator& getEndPosition() const {
            return m_endPosition;
        }

        /**
         * Returns the children matches.
         * @return the children matches.
         */
        const MatchContainer& getChildren() const {
            return m_children;
        }

        /**
         * Returns the source between start and end position.
         * @return the source for this match.
         */
        Source getSource() const {
            return Source(m_startPosition, m_endPosition);
        }

        /**
         * Prints the match tree.
         * @param stream output stream.
         * @param depth tree depth.
         * @param tabSize number of characters for identation.
         * @param sourceMaxCharsPerLine max characters to output from the source, per line.
         */
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
