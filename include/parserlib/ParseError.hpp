#ifndef PARSERLIB_PARSEERROR_HPP
#define PARSERLIB_PARSEERROR_HPP


#include "ParseErrorType.hpp"


namespace parserlib {


    template <class Source>
    class ParseError {
    public:
        typedef Source Source;
        typedef typename Source::const_iterator Iterator;

        ParseError()
            : m_id(-1)
        {
        }

        ParseError(int id, const Iterator& position) 
            : m_id(id)
            , m_position(position)
        {
        }

        int getId() const {
            return m_id;
        }

        const Iterator& getPosition() const {
            return m_position;
        }

    private:
        int m_id;
        Iterator m_position;
    };


} //namespace parserlib


#endif //PARSERLIB_PARSEERROR_HPP
