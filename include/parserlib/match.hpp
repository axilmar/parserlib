#ifndef PARSERLIB_MATCH_HPP
#define PARSERLIB_MATCH_HPP


#include <vector>
#include "source_partition.hpp"


namespace parserlib {


    template <class Id, class ParsePosition>
    class match : public source_partition<Id, ParsePosition> {
    public:
        using id_type = Id;
        using parse_position_type = ParsePosition;
        using match_container_type = std::vector<match<Id, ParsePosition>>;

        match() {
        }

        match(const id_type& id, const parse_position_type& begin_parse_position, const parse_position_type& end_parse_position, match_container_type&& children)
            : source_partition<Id, ParsePosition>(id, begin_parse_position, end_parse_position)
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
