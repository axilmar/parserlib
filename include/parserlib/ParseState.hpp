#ifndef PARSERLIB_PARSESTATE_HPP
#define PARSERLIB_PARSESTATE_HPP


namespace parserlib {


    /**
     * The parse state of a parse context.
     * @param Source source type.
     */
    template <class Source>
    class ParseState {
    public:
        /**
         * The source type.
         */
        typedef Source Source;

        /**
         * The source iterator type.
         */
        typedef typename Source::const_iterator Iterator;

        /**
         * The constructor.
         * @param position current position.
         * @param matchCount number of matches.
         * @param leftRecursionMatchStateCount number of entries of the match state stack.
         */
        ParseState(const Iterator& position, size_t matchCount, size_t leftRecursionMatchStateCount)
            : m_position(position)
            , m_matchCount(matchCount)
            , m_leftRecursionMatchStateCount(leftRecursionMatchStateCount)
        {
        }

        /**
         * Returns the parse position.
         * @return the parse position.
         */
        const Iterator& getPosition() const {
            return m_position;
        }

        /**
         * Returns the match count.
         * @return the match count.
         */
        size_t getMatchCount() const {
            return m_matchCount;
        }

        /**
         * Returns the match state count for left recursion.
         * @return the match state count for left recursion.
         */
        size_t getLeftRecursionMatchStateCount() const {
            return m_leftRecursionMatchStateCount;
        }

    private:
        Iterator m_position;
        size_t m_matchCount;
        size_t m_leftRecursionMatchStateCount;
    };


} //namespace parserlib


#endif //PARSERLIB_PARSESTATE_HPP
