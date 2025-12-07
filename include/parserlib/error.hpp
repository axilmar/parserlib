#ifndef PARSERLIB_ERROR_HPP
#define PARSERLIB_ERROR_HPP


#include "source_partition.hpp"


namespace parserlib {


    template <class Id, class ParsePosition>
    class error_ : public source_partition<Id, ParsePosition> {
    public:
        using id_type = Id;
        using parse_position_type = ParsePosition;
        using source_partition_type = source_partition<Id, ParsePosition>;

        error_(const Id& id = Id(), const ParsePosition& begin_parse_position = ParsePosition(), const ParsePosition& end_parse_position = ParsePosition())
            : source_partition_type(id, begin_parse_position, end_parse_position)
        {
        }
    };


} //namespace parserlib


#endif //PARSERLIB_ERROR_HPP
