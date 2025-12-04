#ifndef PARSERLIB_SOURCE_RANGE_HPP
#define PARSERLIB_SOURCE_RANGE_HPP


#include "container_range.hpp"


namespace parserlib {


    template <class It, class Id, class TextPosition> 
    class source_range : public container_range<It> {
    public:
        source_range(
            const It& begin = It(), 
            const It& end = It(), 
            const Id& id = Id(), 
            const TextPosition& begin_text_position = TextPosition(), 
            const TextPosition& end_text_position = TextPosition()
        )
            : container_range<It>(begin, end)
            , m_id(id)
            , m_begin_text_position(begin_text_position)
            , m_end_text_position(end_text_position)
        {
        }

        const Id& id() const {
            return m_id;
        }

        const TextPosition& begin_text_position() const {
            return m_begin_text_position;
        }

        const TextPosition& end_text_position() const {
            return m_end_text_position;
        }

    private:
        const Id m_id;
        const TextPosition m_begin_text_position;
        const TextPosition m_end_text_position;
    };


} //namespace parserlib


#endif //PARSERLIB_SOURCE_RANGE_HPP
