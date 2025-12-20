#ifndef PARSERLIB_PARSE_ITERATOR_HPP
#define PARSERLIB_PARSE_ITERATOR_HPP


#include <string>
#include "file_text_position.hpp"


namespace parserlib {


    /**
     * A parse iterator.
     * It combines a normal iterator and a text position.
     * Useful in cases where the line and column of the input text needs to be known.
     * @param Iterator iterator type.
     * @param TextPosition text position type.
     */
    template <class Iterator = typename std::string::const_iterator, class TextPosition = file_text_position>
    class parse_iterator {
    public:
        /** Iterator type. */
        using iterator_type = Iterator;

        /** Text position type. */
        using text_position_type = TextPosition;

        /** value type. */
        using value_type = typename iterator_type::value_type;

        /**
         * The constructor.
         * @param it iterator.
         * @param tp text position.
         */
        parse_iterator(const iterator_type& it = iterator_type(), const text_position_type& tp = text_position_type())
            : m_iterator(it)
            , m_text_position(tp)
        {
        }

        /**
         * Returns the iterator.
         * @return the iterator.
         */
        const iterator_type& get_iterator() const noexcept {
            return m_iterator;
        }

        /**
         * Returns the text position.
         * @return the text position.
         */
        const text_position_type& get_text_position() const noexcept {
            return m_text_position;
        }

        /**
         * Checks if this iterator points to the same element as another iterator.
         * @param other the other iterator to compare against this.
         * @return true if both iterators point to the same element, false otherwise.
         */
        bool operator == (const parse_iterator& other) const {
            return m_iterator == other.m_iterator;
        }

        /**
         * Checks if this iterator points to a different element than another iterator.
         * @param other the other iterator to compare against this.
         * @return true if this points to a different element than the given iterator, false otherwise.
         */
        bool operator != (const parse_iterator& other) const {
            return m_iterator != other.m_iterator;
        }

        /**
         * Dereferences the iterator.
         * @return the result of operator *() on the iterator.
         */
        auto operator *() const {
            return *m_iterator;
        }

        /**
         * Increments the parse iterator by one.
         * @return reference to this.
         */
        parse_iterator& operator ++() {
            ++m_iterator;
            ++m_text_position;
            return *this;
        }

        /**
         * Increments the parse iterator by an amount.
         * @param count number of positions to increment the parse position.
         * @return reference to this.
         */
        parse_iterator& operator +=(size_t count) {
            m_iterator = std::next(m_iterator, count);
            m_text_position += count;
            return *this;
        }

        /**
         * Increments the line of the text position.
         * @return reference to this.
         */
        parse_iterator& increment_text_position_line() noexcept {
            m_text_position.increment_line();
            return *this;
        }

    private:
        iterator_type m_iterator;
        text_position_type m_text_position;
    };


} //namespace parserlib


#endif //PARSERLIB_PARSE_ITERATOR_HPP
