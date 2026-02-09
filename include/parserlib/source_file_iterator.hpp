#ifndef PARSERLIB_SOURCE_FILE_ITERATOR_HPP
#define PARSERLIB_SOURCE_FILE_ITERATOR_HPP


#include <iterator>


namespace parserlib {


    /**
     * A source file position. 
     * It maintains a line and a column.
     */ 
    class source_file_position {
    public:
        /**
         * Returns the current line.
         * @return the current line.
         */ 
        size_t get_line() const {
            return m_line;
        }

        /**
         * Returns the current column.
         * @return the current column.
         */ 
        size_t get_column() const {
            return m_column;
        }

        /**
         * Increments the column.
         */ 
        void increment_column() {
            ++m_column;
        }

        /**
         * Increments a column by a specific count.
         * @param count increment count.
         */ 
        void increment_column(size_t count) {
            m_column += count;
        }

        /**
         * Increments the line by 1 and sets the column to 1.
         */ 
        void increment_line() {
            ++m_line;
            m_column = 1;
        }

    private:
        size_t m_line = 1;
        size_t m_column = 1;
    };


    /**
     * A source file iterator.
     * @param Iterator type of iterator.
     * @param SourceFilePosition type of source file position.
     */ 
    template <class Iterator, class SourceFilePosition = source_file_position> 
    class source_file_iterator {
    public:
        /** value type */
        using value_type = typename Iterator::value_type;

        /**
         * The constructor.
         * @param iterator iterator.
         * @param source_file_position the initial source file position.
         */ 
        source_file_iterator(const Iterator& iterator, const SourceFilePosition& source_file_position = {})
            : m_iterator(iterator)
            , m_source_file_position(source_file_position)
        {
        }

        /**
         * Checks if this and the given iterator are equal.
         * @param it the other iterator.
         * @return true if this and the given iterator are equal, false otherwise.
         */ 
        bool operator == (const source_file_iterator& it) const {
            return m_iterator == it.m_iterator;
        }

        /**
         * Checks if this and the given iterator are equal.
         * @param it the other iterator.
         * @return true if this and the given iterator are equal, false otherwise.
         */ 
        bool operator == (const iterator& it) const {
            return m_iterator == it;
        }

        /**
         * Checks if this and the given iterator are different.
         * @param it the other iterator.
         * @return true if this and the given iterator are different, false otherwise.
         */ 
        bool operator != (const source_file_iterator& it) const {
            return m_iterator != it.m_iterator;
        }

        /**
         * Checks if this and the given iterator are different.
         * @param it the other iterator.
         * @return true if this and the given iterator are different, false otherwise.
         */ 
        bool operator != (const iterator& it) const {
            return m_iterator != it;
        }

        /**
         * Returns the value at the specified position.
         * @return the value at the specified position.
         */ 
        typename Iterator::reference operator *() const {
            return *m_iterator;
        }

        /**
         * Increments the iterator and the file position.
         */ 
        void operator ++() {
            ++m_iterator;
            m_source_file_position.increment_column();
        }

        /**
         * Increments the iterator and the file position by a specific count.
         * @param count the increment count.
         */ 
        void operator += (size_t count) {
            m_iterator = std::next(m_iterator, count);
            m_source_file_position.increment_column(count);
        }

        /**
         * Adds a count to a copy of this iterator.
         * @param count the increment count.
         * @return the incremented iterator.
         */ 
        source_file_iterator operator + (size_t count) const {
            source_file_iterator result;
            result += count;
            return result;
        }

        /**
         * Returns the source file position.
         * @return the source file position.
         */ 
        const SourceFilePosition& get_source_file_position() const {
            return m_source_file_position;
        }

        /**
         * Returns the line.
         * @return the line.
         */ 
        size_t get_line() const {
            return m_source_file_position.get_line();
        }

        /**
         * Returns the column.
         * @return the column.
         */ 
        size_t get_column() const {
            return m_source_file_position.get_column();
        }

        /**
         * Increments the line.
         */ 
        void increment_line() {
            m_source_file_position.increment_line();
        }

    private:
        Iterator m_iterator;
        SourceFilePosition m_source_file_position;
    };


} //namespace parserlib


namespace std {


    template <class Iterator, class SourceFilePosition>
    struct iterator_traits<parserlib::source_file_iterator<Iterator, SourceFilePosition>> {
        using difference_type = std::ptrdiff_t;
        using value_type = typename Iterator::value_type;
        using pointer = const value_type*;
        using reference = const value_type&;
        using iterator_category = std::random_access_iterator_tag;
    };


} //namespace std


#endif //PARSERLIB_SOURCE_FILE_ITERATOR_HPP
