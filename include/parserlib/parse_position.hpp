#ifndef PARSERLIB_PARSEPOSITION_HPP
#define PARSERLIB_PARSEPOSITION_HPP


#include <iterator>
#include <type_traits>


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

        bool operator < (const parse_position_type& other) const {
            if constexpr (std::is_same_v<typename Iterator::iterator_category, std::random_access_iterator_tag>) {
                return m_iterator < other.m_iterator;
            }
            else {
                return m_source_position < other.m_source_position;
            }
        }

        void increment() {
            ++m_iterator;
            ++m_source_position;
        }

        void increment(size_t count) {
            std::advance(m_iterator, count);
            m_source_position += count;
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
