#ifndef PARSERLIB_ERROR_HPP
#define PARSERLIB_ERROR_HPP


#include <vector>
#include "source_partition.hpp"


namespace parserlib {


    template <class Id, class ParsePosition>
    class error_ : public source_partition<Id, ParsePosition> {
    public:
        using id_type = Id;
        using parse_position_type = ParsePosition;

        error_() {
        }

        error_(const id_type& id, const parse_position_type& begin_parse_position, const parse_position_type& end_parse_position)
            : source_partition<Id, ParsePosition>(id, begin_parse_position, end_parse_position)
        {
        }
    };


} //namespace parserlib


#endif //PARSERLIB_ERROR_HPP
