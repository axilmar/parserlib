#ifndef PARSERLIB_PARSE_ERROR_HPP
#define PARSERLIB_PARSE_ERROR_HPP


#include "source_range.hpp"


namespace parserlib {


    template <class Id, class Iterator>
    class parse_error : public source_range<Id, Iterator> {
    public:
        /** id type. */
        using id_type = Id;

        /** iterator type. */
        using iterator_type = Iterator;

        /**
         * The default constructor.
         */
        parse_error() noexcept {
        }

        /**
         * The constructor.
         * @param id id of the range.
         * @param begin begin iterator.
         * @param end end iterator.
         */
        parse_error(const id_type& id, const iterator_type& begin, const iterator_type& end)
            : source_range<Id, Iterator>(id, begin, end)
        {
        }
    };


} //namespace parserlib


#endif //PARSERLIB_PARSE_ERROR_HPP
