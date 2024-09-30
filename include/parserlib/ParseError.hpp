#ifndef PARSERLIB_PARSEERROR_HPP
#define PARSERLIB_PARSEERROR_HPP


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
         * @param position the error position.
         */
        ParseError(int id, const Iterator& position) 
            : m_id(id)
            , m_position(position)
        {
        }

        /**
         * Returns the id of the error.
         * @return the id of the error.
         */
        int getId() const {
            return m_id;
        }

        /**
         * Returns the position of the error.
         * @return the position of the error.
         */
        const Iterator& getPosition() const {
            return m_position;
        }

    private:
        int m_id;
        Iterator m_position;
    };


} //namespace parserlib


#endif //PARSERLIB_PARSEERROR_HPP
