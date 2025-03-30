#ifndef PARSERLIB_STRINGS_HPP
#define PARSERLIB_STRINGS_HPP


#include <string>
#include <cstdint>
#include <stdexcept>


namespace parserlib {


    /**
     * An extension of a string that provides a const iterator that knows how to count lines and columns.
     * It does so in operators ++, += and +; all other operators and iterator classes are not managing lines and columns.
     * It is only provided in the context of parsing to allow the identification of the line and column within a source.
     * Lines and columns start counting from 1; line/column 0 are for iterator.end().
     * @param String the base string class.
     * @param LineDelim the character that is the line delimiter.
     */
    template <class String = std::string, typename String::value_type LineDelim = '\n'>
    class line_counting_string : public String {
    public:
        using String::String;

        /**
         * An iterator class that knows how to count lines and columns.
         */
        class const_iterator : public String::const_iterator {
        public:
            using String::const_iterator::const_iterator;

            /**
             * Increments the line and column as needed. 
             * @return reference to this.
             */
            const_iterator& operator ++() noexcept {
                if (*it() != LineDelim) {
                    ++m_column;
                }
                else {
                    ++m_line;
                    m_column = 1;
                }
                ++it();
                return *this;
            }

            /**
             * Increments the line and column as needed (postfix version).
             * @return iterator before the increment.
             */
            const_iterator operator ++(int) noexcept {
                const_iterator result = *this;
                operator ++();
                return result;
            }

            /**
             * Increments the line and column as needed.
             * @param count number of positions to increment the iterator.
             * @return reference to this.
             */
            const_iterator& operator += (std::size_t count) noexcept {
                for (; count > 0; --count) {
                    operator ++();
                }
                return *this;
            }

            /**
             * Increments the line and column as needed.
             * @param count number of positions to increment the iterator.
             * @return an iterator that has been incremented 'count' times.
             */
            const_iterator operator + (std::size_t count) noexcept {
                const_iterator result = *this;
                result += count;
                return result;
            }

            /**
             * Returns the line.
             * @return the line.
             */
            std::size_t line() const noexcept {
                return m_line;
            }

            /**
             * Returns the column.
             * @return the column.
             */
            std::size_t column() const noexcept {
                return m_column;
            }

        private:
            std::size_t m_line;
            std::size_t m_column;
            const_iterator(const typename String::const_iterator& it, std::size_t line, std::size_t column) 
                : String::const_iterator(it), m_line(line), m_column(column) {}
            const typename String::const_iterator& it() const { return *this; }
            typename String::const_iterator& it() { return *this; }
            friend class line_counting_string<String, LineDelim>;
        };

        /**
         * Returns an iterator that points to the first element of the sequence.
         * @return an iterator that points to the first element of the sequence.
         */
        const_iterator begin() const {
            return { String::begin(), 1, 1 };
        }

        /**
         * Returns an iterator that points to the end of the sequence.
         * @return an iterator that points to the end of the sequence.
         */
        const const_iterator end() const {
            return { String::end(), 0, 0 };
        }
    };


    /**
     * An extension of a string which provides a const iterator that knows how to decode UTF8 into unicode code points.
     * Useful for parsing utf8 strings; it does not provide other utf8 functionality.
     * @param String string class to derive from.
     */
    template <class String = std::string>
    class utf8_string : public String {
    public:
        using String::String;

        /**
         * An iterator class that knows how to read unicode code points from a UTF8 encoding.
         */
        class const_iterator : public String::const_iterator {
        public:
            using String::const_iterator::const_iterator;

            /**
             * Returns the unicode codepoint at the specific position.
             * It synthesizes the code point value from 1, 2, 3 or 4 byte values present at the current iterator position.
             * @return current unicode codepoint.
             * @exception std::runtime_error if the unicode code point cannot be decoded due to invalid utf.
             */
            uint32_t operator *() const {
                const uint32_t leader_byte = (unsigned char)*it();

                //single byte (range 0-127)
                if (leader_byte < 128) {
                    return leader_byte;
                }

                if (leader_byte < 192) {
                    goto ERROR;
                }

                //two bytes (range 192-223)
                if (leader_byte < 224) {
                    const uint32_t trailing_byte_1 = (unsigned char) *(it() + 1);
                    const uint32_t result = ((leader_byte - 192) * 64) + (trailing_byte_1 - 128);
                    return result;
                }

                //three bytes (range 224-239)
                if (leader_byte < 240) {
                    const uint32_t trailing_byte_1 = (unsigned char) *(it() + 1);
                    const uint32_t trailing_byte_2 = (unsigned char) *(it() + 2);
                    const uint32_t result = ((leader_byte - 224) * 4096) + ((trailing_byte_1 - 128) * 64) + (trailing_byte_2 - 128);
                    return result;
                }

                //four bytes (range 240-247)
                if (leader_byte < 248) {
                    const uint32_t trailing_byte_1 = (unsigned char) *(it() + 1);
                    const uint32_t trailing_byte_2 = (unsigned char) *(it() + 2);
                    const uint32_t trailing_byte_3 = (unsigned char) *(it() + 3);
                    const uint32_t result = ((leader_byte - 240) * 262144) + ((trailing_byte_1 - 128) * 4096) + ((trailing_byte_2 - 128) * 64) + (trailing_byte_3 - 128);
                    return result;
                }

                ERROR:
                throw std::runtime_error("invalid UTF-8 string");
            }

            /**
             * Increments the iterator by 1, 2, 3 or 4 bytes, depending on the utf8 decoding.
             * @return reference to this.
             * @exception std::runtime_error if the unicode code point cannot be decoded due to invalid utf.
             */
            const_iterator& operator ++() {
                const uint32_t leader_byte = (unsigned char)*it();

                //single byte (range 0-127)
                if (leader_byte < 128) {
                    ++it();
                    return *this;
                }

                if (leader_byte < 192) {
                    goto ERROR;
                }

                //two bytes (range 192-223)
                if (leader_byte < 224) {
                    it() += 2;
                    return *this;
                }

                //three bytes (range 224-239)
                if (leader_byte < 240) {
                    it() += 3;
                    return *this;
                }

                //four bytes (range 240-247)
                if (leader_byte < 248) {
                    it() += 4;
                    return *this;
                }

                return *this;

                ERROR:
                throw std::runtime_error("invalid UTF-8 string");
            }

        private:
            const_iterator(const typename String::const_iterator& it)  : String::const_iterator(it) {}
            const typename String::const_iterator& it() const { return *this; }
            typename String::const_iterator& it() { return *this; }
            friend class utf8_string;
        };

        /**
         * Returns an iterator that points to the first element of the sequence.
         * @return an iterator that points to the first element of the sequence.
         */
        const_iterator begin() const {
            return String::begin();
        }

        /**
         * Returns an iterator that points to the end of the sequence.
         * @return an iterator that points to the end of the sequence.
         */
        const const_iterator end() const {
            return String::end();
        }
    };


} //namespace parserlib


#endif //PARSERLIB_STRINGS_HPP
