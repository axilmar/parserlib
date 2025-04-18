#ifndef PARSERLIB_STRINGS_HPP
#define PARSERLIB_STRINGS_HPP


#include <string>
#include <cstdint>
#include <stdexcept>
#include <cstddef>
#include <iterator>
#include <cassert>
#include <algorithm>


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
        using String::operator =;

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
        using String::operator =;

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


    /**
     * A wrapper over a stream that turns a stream into an STL-like container,
     * suitable for using it for parsing a stream.
     * @param Stream the stream.
     * @param Buffer the container to use for buffer.
     * @param ReadAheadCount number of elements to read from stream when needed.
     */
    template <class Stream, class Buffer = std::basic_string<typename Stream::char_type>, std::size_t ReadAheadCount = 4096>
    class stream_string {
    public:
        static_assert(ReadAheadCount > 0);

        /** Stream wrapper type. */
        using stream_string_type = stream_string<Stream, Buffer, ReadAheadCount>;

        /** Stream type. */
        using stream_type = Stream;

        /** The value type of the container is the character type of the stream. */
        using value_type = typename Stream::char_type;

        /** The traits type of the stream. */
        using traits_type = typename Stream::traits_type;

        /** The stream iterator type. */
        using stream_iterator_type = std::istream_iterator<value_type, value_type, traits_type>;

        /** The buffer type. */
        using buffer_type = Buffer;

        /** An index that signifies the end of the buffer. */
        static std::size_t constexpr end_buffer_index = -1;

        /**
         * The iterator class required for parsing.
         * Only const_iterator is provided, as the elements of the stream are considered unmodifiable.
         */
        class const_iterator {
        public:
            /** Value type. */
            using value_type = typename Stream::char_type;

            /** Difference type. */
            using difference_type = typename Buffer::const_iterator::difference_type;

            /** Pointer type. */
            using pointer = typename Buffer::const_iterator::pointer;

            /** Reference type. */
            using reference = typename Buffer::const_iterator::reference;

            /** Iterator category type. */
            using iterator_category = std::random_access_iterator_tag;

            /**
             * The empty iterator constructor.
             */
            const_iterator() noexcept
                : m_container{}
                , m_buffer_index{}
            {
            }

            /**
             * Returns the value type at the current position.
             * Undefined if the end of the stream is reached.
             * @return the value type at the current position.
             */
            value_type operator *() const noexcept {
                assert(m_buffer_index < m_container->m_buffer.size());
                return m_container->m_buffer[m_buffer_index];
            }

            /**
             * Increments the iterator.
             * New elements are pulled from the stream, if the end of the internal buffer is reached.
             * @return reference to this.
             */
            const_iterator& operator ++() noexcept {
                assert(m_buffer_index <= m_container->m_buffer.size());
                if (m_buffer_index < m_container->m_buffer.size()) {
                    ++m_buffer_index;
                }
                if (m_buffer_index == m_container->m_buffer.size()) {
                    if (!m_container->read_ahead()) {
                        m_buffer_index = -1;
                    }
                }
                return *this;
            }

            /**
             * Advances the iterator `count` positions.
             * @param count number of positions to advance the iterator.
             * @return reference to this.
             */
            const_iterator& operator += (std::size_t count) noexcept {
                for (; count > 0; --count) {
                    operator ++();
                }
                return *this;
            }

            /**
             * Compares this iterator with the given one.
             * @param it the other iterator to compare this to.
             * @return true if the iterators are equal, false otherwise.
             */
            bool operator == (const const_iterator& it) const noexcept {
                check_container(it);
                return m_buffer_index == it.m_buffer_index;
            }

            /**
             * Compares this iterator with the given one.
             * @param it the other iterator to compare this to.
             * @return true if the iterators are different, false otherwise.
             */
            bool operator != (const const_iterator& it) const noexcept {
                check_container(it);
                return m_buffer_index != it.m_buffer_index;
            }

            /**
             * Compares this iterator with the given one.
             * @param it the other iterator to compare this to.
             * @return true if this is less than the given one, false otherwise.
             */
            bool operator < (const const_iterator& it) const noexcept {
                check_container(it);
                return m_buffer_index < it.m_buffer_index;
            }

            /**
             * Compares this iterator with the given one.
             * @param it the other iterator to compare this to.
             * @return true if this is less than or equal to the given one, false otherwise.
             */
            bool operator <= (const const_iterator& it) const noexcept {
                check_container(it);
                return m_buffer_index <= it.m_buffer_index;
            }

            /**
             * Compares this iterator with the given one.
             * @param it the other iterator to compare this to.
             * @return true if this is greater than the given one, false otherwise.
             */
            bool operator > (const const_iterator& it) const noexcept {
                check_container(it);
                return m_buffer_index > it.m_buffer_index;
            }

            /**
             * Compares this iterator with the given one.
             * @param it the other iterator to compare this to.
             * @return true if this is greater than or equal to the given one, false otherwise.
             */
            bool operator >= (const const_iterator& it) const noexcept {
                check_container(it);
                return m_buffer_index >= it.m_buffer_index;
            }

            /**
             * Calculate difference between iterators.
             * @param a first iterator.
             * @param b second iterator.
             * @return their difference.
             */
            friend difference_type operator - (const const_iterator& a, const const_iterator& b) noexcept {
                return static_cast<difference_type>(a.buffer_index - b.m_buffer_index);
            }

            /**
             * Returns the buffer iterator at the current position.
             * @return the buffer iterator at the current position.
             */
            auto buffer_iterator() const noexcept {
                return std::next(m_container->begin(), m_buffer_index);
            }

        private:
            stream_string_type* m_container;
            std::size_t m_buffer_index;

            const_iterator(stream_string_type& container, std::size_t buffer_index) noexcept
                : m_container(&container)
                , m_buffer_index(buffer_index)
            {
            }

            void check_container(const const_iterator& it) const noexcept {
                assert(m_container == it.m_container);
            }

            friend stream_string_type;
        };

        /**
         * The constructor.
         * @param stream stream to wrap.
         */
        stream_string(stream_type& stream) noexcept
            : m_stream(stream)
            , m_buffer{}
        {
            read_ahead();
        }

        /**
         * Returns an iterator to the beginning of the sequence.
         * @return an iterator to the beginning of the sequence.
         */
        const_iterator cbegin() const noexcept {
            return const_iterator(*const_cast<stream_string_type*>(this), 0);
        }

        /**
         * Returns an iterator to the end of the sequence.
         * @return an iterator to the end of the sequence.
         */
        const_iterator cend() const noexcept {
            return const_iterator(*const_cast<stream_string_type*>(this), end_buffer_index);
        }

        /**
         * Returns an iterator to the beginning of the sequence.
         * @return an iterator to the beginning of the sequence.
         */
        const_iterator begin() const noexcept {
            return const_iterator(*const_cast<stream_string_type*>(this), 0);
        }

        /**
         * Returns an iterator to the end of the sequence.
         * @return an iterator to the end of the sequence.
         */
        const_iterator end() const noexcept {
            return const_iterator(*const_cast<stream_string_type*>(this), end_buffer_index);
        }

        /**
         * Returns the stream.
         * @return the stream.
         */
        const stream_type& stream() const noexcept {
            return m_stream;
        }

    private:
        Stream& m_stream;
        buffer_type m_buffer;

        bool read_ahead() noexcept {
            if (!m_stream.eof() && !m_stream.fail()) {
                const std::size_t buffer_pos = m_buffer.size();
                m_buffer.resize(buffer_pos + ReadAheadCount);
                m_stream.read(m_buffer.data() + buffer_pos, ReadAheadCount);
                const std::size_t extracted_char_count = static_cast<std::size_t>(m_stream.gcount());
                if (extracted_char_count > 0 && extracted_char_count <= ReadAheadCount) {
                    m_buffer.resize(buffer_pos + extracted_char_count);
                    return true;
                }
                m_buffer.resize(buffer_pos);
            }
            return false;
        }

        friend const_iterator;
    };


} //namespace parserlib


#endif //PARSERLIB_STRINGS_HPP
