#ifndef PARSERLIB_STRING_WRAPPER_HPP
#define PARSERLIB_STRING_WRAPPER_HPP


#include <cstddef>
#include <string>
#include <utility>


namespace parserlib {


    /**
     * A class that wraps an std::basic_string or a stream_wrapper,
     * providing line and column counting to the parsing.
     * @param String string type.
     */
    template <class String = std::string>
    class string_wrapper {
    public:
        /** String wrapper type. */
        using string_wrapper_type = string_wrapper<String>;

        /** String type. */
        using string_type = String;

        /** Value type. */
        using value_type = typename String::value_type;

        /** String iterator type. */
        using string_iterator_type = typename String::const_iterator;

        /**
         * Const iterator.
         */
        class const_iterator {
        public:
            /** Value type. */
            using value_type = typename String::value_type;

            /** Difference type. */
            using difference_type = typename String::const_iterator::difference_type;

            /** Pointer type. */
            using pointer = typename String::const_iterator::pointer;

            /** Reference type. */
            using reference = typename String::const_iterator::reference;

            /** Iterator category type. */
            using iterator_category = typename String::const_iterator::iterator_category;

            /**
             * The empty iterator constructor.
             */
            const_iterator()
                : m_iterator{}
                , m_line(-1)
                , m_column(-1)
            {
            }

            /**
             * Returns a value at the current position.
             * @return a value at the current position.
             */
            const value_type& operator *() const {
                return *m_iterator;
            }

            /**
             * Increments the internal iterator and column.
             * @return reference to this.
             */
            const_iterator& operator ++() {
                ++m_iterator;
                ++m_column;
                return *this;
            }

            /**
             * Increments the iterator and column by a specific amount of positions.
             * @param count amount of positions to increment the iterator.
             * @return reference to this.
             */
            const_iterator& operator += (std::size_t count) {
                m_iterator += count;
                m_column += count;
                return *this;
            }

            /**
             * Increments the line and sets the column to 1.
             */
            void increment_line() {
                ++m_line;
                m_column = 1;
            }

            /**
             * Compares this iterator with the given one.
             * @param it the other iterator to compare this to.
             * @return true if the iterators are equal, false otherwise.
             */
            bool operator == (const const_iterator& it) const {
                return m_iterator == it.m_iterator;
            }

            /**
             * Compares this iterator with the given one.
             * @param it the other iterator to compare this to.
             * @return true if the iterators are different, false otherwise.
             */
            bool operator != (const const_iterator& it) const {
                return m_iterator != it.m_iterator;
            }

            /**
             * Compares this iterator with the given one.
             * @param it the other iterator to compare this to.
             * @return true if this is less than the given one, false otherwise.
             */
            bool operator < (const const_iterator& it) const {
                return m_iterator < it.m_iterator;
            }

            /**
             * Compares this iterator with the given one.
             * @param it the other iterator to compare this to.
             * @return true if this is less than or equal to the given one, false otherwise.
             */
            bool operator <= (const const_iterator& it) const {
                return m_iterator <= it.m_iterator;
            }

            /**
             * Compares this iterator with the given one.
             * @param it the other iterator to compare this to.
             * @return true if this is greater than the given one, false otherwise.
             */
            bool operator > (const const_iterator& it) const {
                return m_iterator > it.m_iterator;
            }

            /**
             * Compares this iterator with the given one.
             * @param it the other iterator to compare this to.
             * @return true if this is greater than or equal to the given one, false otherwise.
             */
            bool operator >= (const const_iterator& it) const {
                return m_iterator >= m_iterator;
            }

            /**
             * Returns the current line.
             * @return the current line; 1 for the 1st line.
             */
            std::size_t line() const {
                return m_line;
            }

            /**
             * Returns the current column.
             * @return the current column; 1 for the 1st column.
             */
            std::size_t column() const {
                return m_column;
            }

            /**
             * Calculate difference between iterators.
             * @param a first iterator.
             * @param b second iterator.
             * @return their difference.
             */
            friend difference_type operator - (const const_iterator& a, const const_iterator& b) {
                return a.m_iterator - b.m_iterator;
            }

        private:
            string_iterator_type m_iterator;
            std::size_t m_line;
            std::size_t m_column;

            const_iterator(const string_iterator_type& it, std::size_t line, std::size_t column)
                : m_iterator(it)
                , m_line(line)
                , m_column(column)
            {
            }

            friend string_wrapper_type;
        };

        /**
         * The constructor.
         * @param string the string object.
         */
        template <class T>
        string_wrapper(T&& string)
            : m_string(std::forward<T>(string))
        {
        }

        /**
         * Returns an iterator to the beginning of the sequence.
         * @return an iterator to the beginning of the sequence.
         */
        const_iterator cbegin() const {
            return const_iterator(m_string.begin(), 1, 1);
        }

        /**
         * Returns an iterator to the end of the sequence.
         * @return an iterator to the end of the sequence.
         */
        const_iterator cend() const {
            return const_iterator(m_string.end(), -1, -1);
        }

        /**
         * Returns an iterator to the beginning of the sequence.
         * @return an iterator to the beginning of the sequence.
         */
        const_iterator begin() const {
            return const_iterator(m_string.begin(), 1, 1);
        }

        /**
         * Returns an iterator to the end of the sequence.
         * @return an iterator to the end of the sequence.
         */
        const_iterator end() const {
            return const_iterator(m_string.end(), -1, -1);
        }

        /**
         * Returns the string.
         * @return the string.
         */
        const string_type& string() const {
            return m_string;
        }

    private:
        string_type& m_string;
    };


} //namespace parserlib


#endif //PARSERLIB_STRING_WRAPPER_HPP
