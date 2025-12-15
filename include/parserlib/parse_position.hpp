#ifndef PARSERLIB_PARSE_POSITION_HPP
#define PARSERLIB_PARSE_POSITION_HPP


#include <cstddef>


namespace parserlib {


    /**
     * A parse position.
     * It combines an iterator and a source position.
     * @param Iterator type of iterator.
     * @param SourcePosition type of source position.
     */ 
    template <class Iterator, class SourcePosition> 
    class parse_position {
    public:
        /** Iterator type. */
        using iterator_type = Iterator;

        /** Source position type. */
        using source_position_type = SourcePosition;

        /**
         * The default constructor.
         */
        parse_position() {
        }

        /**
         * Constructor from arguments.
         * @param iterator the iterator.
         * @param source_position the source position.
         */
        parse_position(const iterator_type& iterator, const source_position_type& source_position = source_position_type())
            : m_iterator(iterator)
            , m_source_position(source_position)
        {
        }

        /**
         * Returns the iterator.
         * @return the iterator.
         */
        const iterator_type& get_iterator() const {
            return m_iterator;
        }

        /**
         * Returns the index of this parse position.
         * @return the index of this parse position.
         */
        std::size_t get_index() const {
            return m_index;
        }

        /**
         * Returns the source position.
         * @returns the source position.
         */
        const source_position_type& get_source_position() const {
            return m_source_position;
        }

        /**
         * Tests if this and the given parse position are equal.
         * @param other the other parse position to compare to this.
         * @return true if they are equal, false otherwise.
         */
        bool operator == (const parse_position& other) const {
            return m_index == other.m_index;
        }

        /**
         * Tests if this and the given parse position are different.
         * @param other the other parse position to compare to this.
         * @return true if they are different, false otherwise.
         */
        bool operator != (const parse_position& other) const {
            return m_index != other.m_index;
        }

        /**
         * Tests if this parse position comes before the given parse position.
         * @param other the other parse position to compare to this.
         * @return true if this parse position comes before the given parse position, false otherwise.
         */
        bool operator < (const parse_position& other) const {
            return m_index < other.m_index;
        }

        /**
         * Tests if this parse position comes before the given parse position or if they are equal.
         * @param other the other parse position to compare to this.
         * @return true if this parse position comes before the given parse position or they are equal, false otherwise.
         */
        bool operator <= (const parse_position& other) const {
            return m_index <= other.m_index;
        }

        /**
         * Tests if this parse position comes after the given parse position.
         * @param other the other parse position to compare to this.
         * @return true if this parse position comes after the given parse position, false otherwise.
         */
        bool operator > (const parse_position& other) const {
            return m_index < other.m_index;
        }

        /**
         * Tests if this parse position comes after the given parse position or if they are equal.
         * @param other the other parse position to compare to this.
         * @return true if this parse position comes after the given parse position or they are equal, false otherwise.
         */
        bool operator >= (const parse_position& other) const {
            return m_index <= other.m_index;
        }

        /**
         * Increments this parse position by one.
         */
        void increment() {
            ++m_iterator;
            ++m_index;
            m_source_position.increment();
        }

        /**
         * Increments this parse position by a number.
         * @param count number of positions to advance.
         */
        void increment(std::size_t count) {
            m_iterator = std::next(m_iterator, count);
            m_index += count;
            m_source_position.increment(count);
        }

        /**
         * Increments the line of the source position.
         */
        void increment_line() {
            m_source_position.increment_line();
        }

    private:
        iterator_type m_iterator;
        std::size_t m_index{ 0 };
        source_position_type m_source_position;
    };


} //namespace parserlib


#endif //PARSERLIB_PARSE_POSITION_HPP
