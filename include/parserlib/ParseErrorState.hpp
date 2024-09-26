#ifndef PARSERLIB_PARSEERRORSTATE_HPP
#define PARSERLIB_PARSEERRORSTATE_HPP


namespace parserlib {


    class ParseErrorState {
    public:
        ParseErrorState(size_t errorsSize, size_t committedErrorsSize)
            : m_errorsSize(errorsSize)
            , m_committedErrorsSize(committedErrorsSize)
        {
        }

        size_t getErrorsSize() const {
            return m_errorsSize;
        }

        size_t getCommittedErrorsSize() const {
            return m_committedErrorsSize;
        }

    private:
        size_t m_errorsSize;
        size_t m_committedErrorsSize;
    };


} //namespace parserlib


#endif //PARSERLIB_PARSEERRORSTATE_HPP
