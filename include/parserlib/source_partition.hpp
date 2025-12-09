#ifndef PARSERLIB_SOURCE_PARTITION_HPP
#define PARSERLIB_SOURCE_PARTITION_HPP


#include "parse_position.hpp"


namespace parserlib {


    template <class Id, class ParsePosition>
    class source_partition {
    public:
        using id_type = Id;
        using parse_position_type = ParsePosition;

        source_partition() 
            : m_id()
            , m_begin_parse_position()
            , m_end_parse_position()
        {
        }

        source_partition(const id_type& id, const parse_position_type& begin_parse_position, const parse_position_type& end_parse_position)
            : m_id(id)
            , m_begin_parse_position(begin_parse_position)
            , m_end_parse_position(end_parse_position)
        {
        }

        const id_type& get_id() const {
            return m_id;
        }

        const parse_position_type& get_begin_parse_position() const {
            return m_begin_parse_position;
        }

        const parse_position_type& get_end_parse_position() const {
            return m_end_parse_position;
        }

    private:
        const id_type m_id;
        const parse_position_type m_begin_parse_position;
        const parse_position_type m_end_parse_position;
    };


} //namespace parserlib


#endif //PARSERLIB_SOURCE_PARTITION_HPP
