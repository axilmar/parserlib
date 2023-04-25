#ifndef PARSERLIB_MATCH_HPP
#define PARSERLIB_MATCH_HPP


namespace parserlib {


    /**
     * Result of a successful parsing attempt.
     * @param SourceType container with source data.
     * @param MatchIdType id to apply to a match.
     * @param PositionType type of source position.
     */
    template <class SourceType, class MatchIdType, class PositionType> class Match {
    public:
        /**
         * The default constructor.
         * No member is initialized.
         */
        Match() {
        }

        /**
         * Constructor from parameters.
         * @param id id of match.
         * @param begin begin position of match.
         * @param end end position of match.
         * @param children children matches.
         */
        Match(const MatchIdType& id,
            const PositionType& begin,
            const PositionType& end,
            std::vector<Match>&& children = std::vector<Match>())
            : m_id(id), m_begin(begin), m_end(end), m_children(std::move(children))
        {
        }

        /**
         * Returns the id of the match.
         * @return the id of the match.
         */
        const MatchIdType& id() const {
            return m_id;
        }

        /**
         * Returns the position the match begins.
         * @return the position the match begins.
         */
        const PositionType& begin() const {
            return m_begin;
        }

        /**
         * Returns the position the match ends.
         * @return the position the match ends.
         */
        const PositionType& end() const {
            return m_end;
        }

        /**
         * Returns the parsed content.
         * @return the parsed content.
         */
        SourceType content() const {
            return SourceType(m_begin.iterator(), m_end.iterator());
        }

        /**
         * Returns the children matches.
         * @return the children matches.
         */
        const std::vector<Match>& children() const {
            return m_children;
        }

    private:
        const MatchIdType m_id{};
        const PositionType m_begin;
        const PositionType m_end;
        const std::vector<Match> m_children;
    };


} //namespace parserlib


#endif //PARSERLIB_MATCH_HPP
