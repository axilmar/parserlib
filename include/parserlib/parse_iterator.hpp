#ifndef PARSERLIB_PARSE_ITERATOR_HPP
#define PARSERLIB_PARSE_ITERATOR_HPP


#include <string>
#include "file_text_position.hpp"


namespace parserlib {


    /**
     * A parse iterator.
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
         * Returns the index of this parse position.
         * @return the index of this parse position.
         */
        operator size_t () const noexcept {
            return m_index;
        }

        /**
         * Returns the iterator.
         * @return the iterator.
         */
        const iterator_type& get_iterator() const noexcept {
            return m_iterator;
        }

        /**
         * Dereferences the iterator.
         * @return the result of operator *() on the iterator.
         */
        auto operator *() const {
            return *m_iterator;
        }

        /**
         * Returns the text position.
         * @return the text position.
         */
        const text_position_type& get_text_position() const noexcept {
            return m_text_position;
        }

        /**
         * Increments the parse position.
         */
        void operator ++() {
            ++m_index;
            ++m_iterator;
            ++m_text_position;
        }

        /**
         * Increments the parse position by an amount.
         * @param count number of positions to increment the parse position.
         */
        void operator +=(size_t count) {
            m_index += count;
            m_iterator = std::next(m_iterator, count);
            m_text_position += count;
        }

        /**
         * Increments the line of the text position.
         */
        void increment_text_position_line() {
            m_text_position.increment_line();
        }

    private:
        size_t m_index{ 0 };
        iterator_type m_iterator;
        text_position_type m_text_position;
    };


} //namespace parserlib


#endif //PARSERLIB_PARSE_ITERATOR_HPP
