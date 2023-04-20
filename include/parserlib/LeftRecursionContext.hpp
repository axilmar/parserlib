#ifndef PARSERLIB_LEFTRECURSIONCONTEXT_HPP
#define PARSERLIB_LEFTRECURSIONCONTEXT_HPP


namespace parserlib {


    template <class ParseContextType> class LeftRecursionContext {
    public:
        using PositionType = typename ParseContextType::PositionType;

        LeftRecursionContext(const PositionType& startPosition, const size_t startMatchCount)
            : m_startPosition(startPosition), m_startMatchCount(startMatchCount)
        {
        }

        const PositionType& startPosition() const {
            return m_startPosition;
        }

        size_t startMatchCount() const {
            return m_startMatchCount;
        }

        bool continuationResolved() const {
            return m_continuationResolved;
        }

        void setContinuationResolved(bool v) {
            m_continuationResolved = v;
        }

    private:
        const PositionType m_startPosition;
        const size_t m_startMatchCount;
        bool m_continuationResolved{ false };
    };


} //namespace parserlib


#endif //PARSERLIB_LEFTRECURSIONCONTEXT_HPP
