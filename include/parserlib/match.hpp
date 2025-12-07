#ifndef PARSERLIB_MATCH_HPP
#define PARSERLIB_MATCH_HPP


#include <vector>
#include <utility>
#include "source_partition.hpp"


namespace parserlib {


    template <class Id, class ParsePosition>
    class match : public source_partition<Id, ParsePosition> {
    public:
        using id_type = Id;
        using parse_position_type = ParsePosition;
        using match_type = match<Id, ParsePosition>;
        using source_partition_type = source_partition<Id, ParsePosition>;
        using match_container_type = std::vector<match_type>;

        match(const Id& id = Id(), const ParsePosition& begin_parse_position = ParsePosition(), const ParsePosition& end_parse_position = ParsePosition(), match_container_type&& children = match_container_type())
            : source_partition_type(id, begin_parse_position, end_parse_position)
            , m_children(std::move(children))
        {
        }

        const match_container_type& get_children() const {
            return m_children;
        }

    private:
        const match_container_type m_children;
    };


} //namespace parserlib


#endif //PARSERLIB_MATCH_HPP
