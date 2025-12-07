#ifndef PARSERLIB_PARSEPOSITION_HPP
#define PARSERLIB_PARSEPOSITION_HPP


#include <iterator>


namespace parserlib {


    template <class Iterator, class SourcePosition>
    class parse_position {
    public:
        using iterator_type = Iterator;
        using source_position_type = SourcePosition;
        using parse_position_type = parse_position<Iterator, SourcePosition>;

        parse_position(const Iterator& iterator = Iterator(), const SourcePosition& source_position = SourcePosition())
            : m_iterator(iterator)
            , m_source_position(source_position)
        {
        }

        const Iterator& get_iterator() const {
            return m_iterator;
        }

        const SourcePosition& get_source_position() const {
            return m_source_position;
        }

        bool operator == (const parse_position_type& other) const {
            return m_iterator == other.m_iterator;
        }

        bool operator != (const parse_position_type& other) const {
            return m_iterator != other.m_iterator;
        }

        bool operator < (const parse_position_type& other) const {
            return m_source_position < other.m_source_position;
        }

        bool operator <= (const parse_position_type& other) const {
            return m_source_position <= other.m_source_position;
        }

        bool operator > (const parse_position_type& other) const {
            return m_source_position > other.m_source_position;
        }

        bool operator >= (const parse_position_type& other) const {
            return m_source_position >= other.m_source_position;
        }

        void set_iterator(const Iterator& iterator) {
            m_iterator = iterator;
        }

        void set_source_position(const SourcePosition& source_position) {
            m_source_position = source_position;
        }

        parse_position_type& operator ++() {
            ++m_iterator;
            ++m_source_position;
            return *this;
        }

        parse_position_type& operator +=(size_t count) {
            std::advance(m_iterator, count);
            m_source_position += count;
            return *this;
        }

        void increment_line() {
            m_source_position.increment_line();
        }

    private:
        Iterator m_iterator;
        SourcePosition m_source_position;
    };


} //namespace parserlib


#endif //PARSERLIB_PARSEPOSITION_HPP
