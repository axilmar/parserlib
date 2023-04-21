#ifndef PARSERLIB_LEFTRECURSIONCONTEXT_HPP
#define PARSERLIB_LEFTRECURSIONCONTEXT_HPP


namespace parserlib {


    /**
     * Variables required for left recursion parsing.
     * @param ParseContextType type of parse context.
     */
    template <class ParseContextType> class LeftRecursionContext {
    public:
        /**
         * Position type.
         */
        using PositionType = typename ParseContextType::PositionType;

        /**
         * The constructor.
         * @param startPosition start position.
         * @param startMatchCount number of matches currently in the parse context.
         */
        LeftRecursionContext(const PositionType& startPosition, const size_t startMatchCount)
            : m_startPosition(startPosition), m_startMatchCount(startMatchCount)
        {
        }

        /**
         * Returns the position left recursion started.
         * @return the position left recursion started.
         */
        const PositionType& startPosition() const {
            return m_startPosition;
        }

        /**
         * Returns the number of matches when left recursion started.
         * @return the number of matches when left recursion started.
         */
        size_t startMatchCount() const {
            return m_startMatchCount;
        }

        /**
         * Returns true if a left recursive rule has been invoked to parse
         * within a left recursion context, after the non-left-recursive part
         * has successfully been parsed; otherwise, it returns false.
         * @return true if the continuation is resolved, false otherwise.
         */
        bool continuationResolved() const {
            return m_continuationResolved;
        }

        /**
         * Sets the continuation resolved property.
         * @param v the property's value.
         */
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
