#ifndef PARSERLIB_STREAM_WRAPPER_HPP
#define PARSERLIB_STREAM_WRAPPER_HPP


#include <cstddef>
#include <iterator>
#include <vector>
#include <cassert>


namespace parserlib {


    /**
     * A wrapper over a stream that turns a stream into an STL-like container,
     * suitable for using it for parsing a stream.
     * @param Stream the stream.
     * @param Buffer the container to use for buffer.
     * @param ReadAheadCount number of elements to read from stream when needed.
     */
    template <class Stream, class Buffer = std::vector<typename Stream::char_type>, std::size_t ReadAheadCount = 4096>
    class stream_wrapper {
    public:
        static_assert(ReadAheadCount > 0);

        /** Stream wrapper type. */
        using stream_wrapper_type = stream_wrapper<Stream>;

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
            /**
             * The empty iterator constructor.
             */
            const_iterator()
                : m_container{}
                , m_buffer_index{}
            {
            }

            /**
             * Returns the value type at the current position.
             * Undefined if the end of the stream is reached.
             * @return the value type at the current position.
             */
            const value_type& operator *() const noexcept {
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
                    if (!m_container->readAhead()) {
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

        private:
            stream_wrapper_type* m_container;
            std::size_t m_buffer_index;

            const_iterator(stream_wrapper_type& container, std::size_t buffer_index) noexcept
                : m_container(&container)
                , m_buffer_index(buffer_index)
            {
            }

            void check_container(const const_iterator& it) const noexcept {
                assert(m_container == it.m_container);
            }

            friend stream_wrapper_type;
        };

        /**
         * The constructor.
         * @param stream stream to wrap.
         */
        stream_wrapper(stream_type& stream) noexcept
            : m_stream(stream)
            , m_buffer{}
        {
            readAhead();
        }

        /**
         * Returns an iterator to the beginning of the sequence.
         * @return an iterator to the beginning of the sequence.
         */
        const_iterator cbegin() const noexcept {
            return const_iterator(*const_cast<stream_wrapper_type*>(this), 0);
        }

        /**
         * Returns an iterator to the end of the sequence.
         * @return an iterator to the end of the sequence.
         */
        const_iterator cend() const noexcept {
            return const_iterator(*const_cast<stream_wrapper_type*>(this), end_buffer_index);
        }

        /**
         * Returns an iterator to the beginning of the sequence.
         * @return an iterator to the beginning of the sequence.
         */
        const_iterator begin() const noexcept {
            return const_iterator(*const_cast<stream_wrapper_type*>(this), 0);
        }

        /**
         * Returns an iterator to the end of the sequence.
         * @return an iterator to the end of the sequence.
         */
        const_iterator end() const noexcept {
            return const_iterator(*const_cast<stream_wrapper_type*>(this), end_buffer_index);
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

        friend const_iterator;

        bool readAhead() {
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
    };


} //namespace parserlib


#endif //PARSERLIB_STREAM_WRAPPER_HPP
