#ifndef PARSERLIB_PARSESTATE_HPP
#define PARSERLIB_PARSESTATE_HPP


namespace parserlib {


    template <class Source>
    class ParseState {
    public:
        typedef Source Source;
        typedef typename Source::const_iterator Iterator;

        ParseState(const Iterator& position, size_t matchCount, size_t leftRecursionMatchStateCount)
            : m_position(position)
            , m_matchCount(matchCount)
            , m_leftRecursionMatchStateCount(leftRecursionMatchStateCount)
        {
        }

        const Iterator& getPosition() const {
            return m_position;
        }

        size_t getMatchCount() const {
            return m_matchCount;
        }

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
