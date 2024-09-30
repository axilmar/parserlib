#ifndef PARSERLIB_PARSEERRORSTATE_HPP
#define PARSERLIB_PARSEERRORSTATE_HPP


namespace parserlib {


    /**
     * The error state of a parse context.
     */
    class ParseErrorState {
    public:
        /**
         * The constructor.
         * @param errorsSize size of the error container.
         * @param committedErrorsSize number of errors committed.
         */
        ParseErrorState(size_t errorsSize, size_t committedErrorsSize)
            : m_errorsSize(errorsSize)
            , m_committedErrorsSize(committedErrorsSize)
        {
        }

        /**
         * Returns the error count.
         * @return the error count.
         */
        size_t getErrorsSize() const {
            return m_errorsSize;
        }

        /**
         * Returns the committed errors count.
         * @return the committed errors count.
         */
        size_t getCommittedErrorsSize() const {
            return m_committedErrorsSize;
        }

    private:
        size_t m_errorsSize;
        size_t m_committedErrorsSize;
    };


} //namespace parserlib


#endif //PARSERLIB_PARSEERRORSTATE_HPP
