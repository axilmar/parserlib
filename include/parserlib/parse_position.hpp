#ifndef PARSERLIB_PARSE_POSITION_HPP
#define PARSERLIB_PARSE_POSITION_HPP


namespace parserlib {


    template <class Source, class SourcePosition> 
    class parse_position {
    public:
        using source_type = Source;
        using iterator_type = typename Source::const_iterator;
        using source_position_type = SourcePosition;

        parse_position() {
        }

        parse_position(const iterator_type& iterator, const source_position_type& source_position)
            : m_iterator(iterator)
            , m_source_position(source_position)
        {
        }

        const iterator_type& get_iterator() const {
            return m_iterator;
        }

        const source_position_type& get_source_position() const {
            return m_source_position;
        }

        bool operator == (const parse_position& other) const {
            return m_source_position == other.m_source_position;
        }

        bool operator != (const parse_position& other) const {
            return m_source_position != other.m_source_position;
        }

        bool operator < (const parse_position& other) const {
            return m_source_position < other.m_source_position;
        }

        bool operator <= (const parse_position& other) const {
            return m_source_position <= other.m_source_position;
        }

        bool operator > (const parse_position& other) const {
            return m_source_position > other.m_source_position;
        }

        bool operator >= (const parse_position& other) const {
            return m_source_position >= other.m_source_position;
        }

        void increment() {
            ++m_iterator;
            m_source_position.increment();
        }

        void increment_line() {
            m_source_position.increment_line();
        }

    private:
        iterator_type m_iterator;
        source_position_type m_source_position;
    };


} //namespace parserlib


#endif //PARSERLIB_PARSE_POSITION_HPP
