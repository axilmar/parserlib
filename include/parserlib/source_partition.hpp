#ifndef PARSERLIB_SOURCE_PARTITION_HPP
#define PARSERLIB_SOURCE_PARTITION_HPP


#include <cassert>
#include <type_traits>


namespace parserlib {


    struct source_partition_tag {
    };


    template <class Id, class ParsePosition>
    class source_partition : public source_partition_tag {
    public:
        using id_type = Id;
        using parse_position_type = ParsePosition;

        source_partition(const Id& id = Id(), const ParsePosition& begin_parse_position = ParsePosition(), const ParsePosition& end_parse_position = ParsePosition())
            : m_id(id)
            , m_begin_parse_position(begin_parse_position)
            , m_end_parse_position(end_parse_position)
        {
            assert(m_begin_parse_position < m_end_parse_position);
        }

        const Id& get_id() const {
            return m_id;
        }

        const ParsePosition& get_begin_parse_position() const {
            return m_begin_parse_position;
        }

        const ParsePosition& get_end_parse_position() const {
            return m_end_parse_position;
        }

        template <class Container>
        Container get_source() const {
            return Container(m_begin_parse_position.get_iterator(), m_end_parse_position.get_iterator());
        }

        template <class Container>
        Container get_text() const {
            return Container(get_text_iterator(m_begin_parse_position.get_iterator()), get_text_iterator(m_end_parse_position.get_iterator()));
        }

    private:
        const Id m_id;
        const ParsePosition m_begin_parse_position;
        const ParsePosition m_end_parse_position;

        template <class Iterator> 
        static const auto& get_text_iterator(const Iterator& iterator) {
            if constexpr (std::is_base_of_v<source_partition_tag, typename Iterator::value_type>) {
                return get_text_iterator(iterator->get_begin_parse_position().get_iterator());
            }
            else {
                return iterator;
            }
        }
    };


} //namespace parserlib


#endif //PARSERLIB_SOURCE_PARTITION_HPP
