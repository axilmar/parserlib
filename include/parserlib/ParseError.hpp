#ifndef PARSERLIB_PARSEERROR_HPP
#define PARSERLIB_PARSEERROR_HPP


#include <cassert>
#include "ParseErrorType.hpp"


namespace parserlib {


    /**
     * A parse error.
     * 
     * @param Source the source type.
     */
    template <class Source>
    class ParseError {
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
         * The default constructor.
         * The error id defaults to -1.
         */
        ParseError()
            : m_id(-1)
        {
        }

        /**
         * The constructor.
         * @param id the error id.
         * @param startPosition the start position.
         * @param endPosition the end position.
         */
        template <class ErrorType>
        ParseError(ErrorType id, const Iterator& startPosition, const Iterator& endPosition) 
            : m_id(static_cast<int>(id))
            , m_startPosition(startPosition)
            , m_endPosition(endPosition)
        {
            assert(startPosition <= endPosition);
        }

        /**
         * Returns the id of the error.
         * @return the id of the error.
         */
        int getId() const {
            return m_id;
        }

        /**
         * Sets the id of the error.
         * @param id id of the error.
         */
        void setId(int id) {
            m_id = id;
        }

        /**
         * Returns the start position of the error.
         * @return the start position of the error.
         */
        const Iterator& getStartPosition() const {
            return m_startPosition;
        }

        /**
         * Returns the end position of the error.
         * @return the end position of the error.
         */
        const Iterator& getEndPosition() const {
            return m_endPosition;
        }

    private:
        int m_id;
        Iterator m_startPosition;
        Iterator m_endPosition;
    };


} //namespace parserlib


#endif //PARSERLIB_PARSEERROR_HPP
