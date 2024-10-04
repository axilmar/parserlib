#ifndef PARSERLIB_CORE_PARSEERROR_HPP
#define PARSERLIB_CORE_PARSEERROR_HPP


#include <cassert>
#include "ParseErrorType.hpp"


namespace parserlib::core {


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

        /**
         * Compares this error with another for equality.
         * @param error error to compare this with.
         * @return true if they are equal (starting and ending position), false otherwise.
         */
        bool operator == (const ParseError& error) const {
            return m_startPosition == error.m_startPosition && m_endPosition == error.m_endPosition;
        }

        /**
         * Compares this error with another for difference.
         * @param error error to compare this with.
         * @return true if they are not equal (starting or ending position), false otherwise.
         */
        bool operator != (const ParseError& error) const {
            return !operator == (error);
        }

        /**
         * Compares this error with another for position.
         * Shorter errors come first.
         * @param error error to compare this with.
         * @return true if this preceeds the given error, false otherwise.
         */
        bool operator < (const ParseError& error) const {
            if (m_startPosition < error.m_startPosition) {
                return true;
            }
            if (m_startPosition > error.m_startPosition) {
                return false;
            }
            if (m_endPosition < error.m_endPosition) {
                return true;
            }
            return false;
        }

        /**
         * Compares this error with another for position.
         * Shorter errors come first.
         * @param error error to compare this with.
         * @return true if this preceeds the given error or if they are equal, false otherwise.
         */
        bool operator <= (const ParseError& error) const {
            if (m_startPosition < error.m_startPosition) {
                return true;
            }
            if (m_startPosition > error.m_startPosition) {
                return false;
            }
            if (m_endPosition < error.m_endPosition) {
                return true;
            }
            if (m_endPosition > error.m_endPosition) {
                return false;
            }
            return true;
        }

        /**
         * Compares this error with another for position.
         * Shorter errors come first.
         * @param error error to compare this with.
         * @return true if this succeeds the given error, false otherwise.
         */
        bool operator > (const ParseError& error) const {
            if (m_startPosition > error.m_startPosition) {
                return true;
            }
            if (m_startPosition < error.m_startPosition) {
                return false;
            }
            if (m_endPosition > error.m_endPosition) {
                return true;
            }
            return false;
        }

        /**
         * Compares this error with another for position.
         * Shorter errors come first.
         * @param error error to compare this with.
         * @return true if this succeeds the given error or if they are equal, false otherwise.
         */
        bool operator >= (const ParseError& error) const {
            if (m_startPosition > error.m_startPosition) {
                return true;
            }
            if (m_startPosition < error.m_startPosition) {
                return false;
            }
            if (m_endPosition > error.m_endPosition) {
                return true;
            }
            if (m_endPosition < error.m_endPosition) {
                return false;
            }
            return true;
        }

    private:
        int m_id;
        Iterator m_startPosition;
        Iterator m_endPosition;
    };


} //namespace parserlib::core


#endif //PARSERLIB_CORE_PARSEERROR_HPP
