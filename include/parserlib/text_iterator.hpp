#ifndef PARSERLIB_TEXT_ITERATOR_HPP
#define PARSERLIB_TEXT_ITERATOR_HPP


#include <iterator>
#include <string>


namespace parserlib {


    /**
     * A text position. 
     * It maintains a line and a column.
     */ 
    class text_position {
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
     * A text iterator.
     * @param Iterator type of iterator.
     * @param TextPosition type of text position.
     */ 
    template <class Iterator = std::string::const_iterator, class TextPosition = text_position> 
    class text_iterator {
    public:
        /** The iterator type. */
        using iterator_type = Iterator;

        /** The value type. */
        using value_type = typename Iterator::value_type;

        /** The text position type. */
        using text_position_type = TextPosition;

        /**
         * The constructor.
         * @param iterator iterator.
         * @param text_position the initial text position.
         */ 
        text_iterator(const Iterator& iterator = Iterator(), const TextPosition& text_position = {})
            : m_iterator(iterator)
            , m_text_position(text_position)
        {
        }

        /**
         * Checks if this and the given iterator are equal.
         * @param it the other iterator.
         * @return true if this and the given iterator are equal, false otherwise.
         */ 
        bool operator == (const text_iterator& it) const {
            return m_iterator == it.m_iterator;
        }

        /**
         * Checks if this and the given iterator are equal.
         * @param it the other iterator.
         * @return true if this and the given iterator are equal, false otherwise.
         */ 
        bool operator == (const Iterator& it) const {
            return m_iterator == it;
        }

        /**
         * Checks if this and the given iterator are different.
         * @param it the other iterator.
         * @return true if this and the given iterator are different, false otherwise.
         */ 
        bool operator != (const text_iterator& it) const {
            return m_iterator != it.m_iterator;
        }

        /**
         * Checks if this and the given iterator are different.
         * @param it the other iterator.
         * @return true if this and the given iterator are different, false otherwise.
         */ 
        bool operator != (const Iterator& it) const {
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
            m_text_position.increment_column();
        }

        /**
         * Increments the iterator and the file position by a specific count.
         * @param count the increment count.
         */ 
        void operator += (size_t count) {
            m_iterator = std::next(m_iterator, count);
            m_text_position.increment_column(count);
        }

        /**
         * Adds a count to a copy of this iterator.
         * @param count the increment count.
         * @return the incremented iterator.
         */ 
        text_iterator operator + (size_t count) const {
            text_iterator result;
            result += count;
            return result;
        }

        /**
         * Returns the text position.
         * @return the text position.
         */ 
        const TextPosition& get_text_position() const {
            return m_text_position;
        }

        /**
         * Returns the line.
         * @return the line.
         */ 
        size_t get_line() const {
            return m_text_position.get_line();
        }

        /**
         * Returns the column.
         * @return the column.
         */ 
        size_t get_column() const {
            return m_text_position.get_column();
        }

        /**
         * Increments the line.
         */ 
        void increment_line() {
            m_text_position.increment_line();
        }

    private:
        Iterator m_iterator;
        TextPosition m_text_position;
    };


} //namespace parserlib


namespace std {


    template <class Iterator, class TextPosition>
    struct iterator_traits<parserlib::text_iterator<Iterator, TextPosition>> {
        using difference_type = std::ptrdiff_t;
        using value_type = typename Iterator::value_type;
        using pointer = const value_type*;
        using reference = const value_type&;
        using iterator_category = std::random_access_iterator_tag;
    };


} //namespace std


#endif //PARSERLIB_TEXT_ITERATOR_HPP
