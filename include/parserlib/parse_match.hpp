#ifndef PARSERLIB_PARSE_MATCH_HPP
#define PARSERLIB_PARSE_MATCH_HPP


#include <vector>
#include "source_range.hpp"


namespace parserlib {


    template <class It, class Id, class TextPosition> class parse_match : public source_range<It, Id, TextPosition> {
    public:
        using parse_match_type = parse_match<It, Id, TextPosition>;
        using parse_match_container_type = std::vector<parse_match_type>;

        parse_match(const It& begin = It(), 
            const It& end = It(), 
            const Id& id = Id(), 
            const TextPosition& begin_text_position = TextPosition(), 
            const TextPosition& end_text_position = TextPosition(),
            parse_match_container_type&& children = parse_match_container_type()
        )
            : source_range<It, Id, TextPosition>(begin, end, id, begin_text_position, end_text_position)
            , m_children(std::move(children))
        {
        }

        const parse_match_container_type& children() const {
            return m_children;
        }

    private:
        const parse_match_container_type m_children;
    };


} //namespace parserlib


#endif //PARSERLIB_PARSE_MATCH_HPP
