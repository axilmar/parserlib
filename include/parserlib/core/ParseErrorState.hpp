#ifndef PARSERLIB_CORE_PARSEERRORSTATE_HPP
#define PARSERLIB_CORE_PARSEERRORSTATE_HPP


namespace parserlib::core {


    /**
     * The error state of a parse context.
     */
    class ParseErrorState {
    public:
        /**
         * The constructor.
         * @param errorsSize size of the error container.
         */
        ParseErrorState(size_t errorsSize)
            : m_errorsSize(errorsSize)
        {
        }

        /**
         * Returns the error count.
         * @return the error count.
         */
        size_t getErrorsSize() const {
            return m_errorsSize;
        }

    private:
        size_t m_errorsSize;
    };


} //namespace parserlib::core


#endif //PARSERLIB_CORE_PARSEERRORSTATE_HPP
