#ifndef PARSERLIB_STREAM_WRAPPER_HPP
#define PARSERLIB_STREAM_WRAPPER_HPP


#include <cstddef>
#include <iterator>
#include <vector>
#include <cassert>


namespace parserlib {


    template <class Stream, class BufferType = std::vector<typename Stream::char_type>>
    class stream_wrapper {
    public:
        using stream_wrapper_type = stream_wrapper<Stream>;
        using stream_type = Stream;
        using value_type = typename Stream::char_type;
        using traits_type = typename Stream::traits_type;
        using stream_iterator_type = std::istream_iterator<value_type, value_type, traits_type>;
        using buffer_type = BufferType;

        static std::size_t constexpr end_buffer_index = -1;

        class const_iterator {
        public:
            const_iterator()
                : m_container{}
                , m_buffer_index{}
            {
            }

            value_type operator *() const noexcept {
                assert(m_buffer_index < m_container->m_buffer.size());
                return m_container->m_buffer[m_buffer_index];
            }

            const_iterator& operator ++() noexcept {
                assert(m_buffer_index <= m_container->m_buffer.size());
                if (m_buffer_index < m_container->m_buffer.size()) {
                    ++m_buffer_index;
                }
                if (m_buffer_index == m_container->m_buffer.size()) {
                    ++m_container->m_iterator;
                    if (m_container->m_iterator != m_container->m_end_iterator) {
                        m_container->m_buffer.push_back(*m_container->m_iterator);
                    }
                    else {
                        m_buffer_index = end_buffer_index;
                    }
                }
                return *this;
            }

            bool operator == (const const_iterator& it) const noexcept {
                check_container(it);
                return m_buffer_index == it.m_buffer_index;
            }

            bool operator != (const const_iterator& it) const noexcept {
                check_container(it);
                return m_buffer_index != it.m_buffer_index;
            }

            bool operator < (const const_iterator& it) const noexcept {
                check_container(it);
                return m_buffer_index < it.m_buffer_index;
            }

            bool operator <= (const const_iterator& it) const noexcept {
                check_container(it);
                return m_buffer_index <= it.m_buffer_index;
            }

            bool operator > (const const_iterator& it) const noexcept {
                check_container(it);
                return m_buffer_index > it.m_buffer_index;
            }

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

        stream_wrapper(stream_type& stream) noexcept
            : m_iterator(stream)
            , m_buffer(init_buffer(m_iterator, m_end_iterator))
        {            
        }

        const_iterator begin() const noexcept {
            return const_iterator(*const_cast<stream_wrapper_type*>(this), 0);
        }

        const_iterator end() const noexcept {
            return const_iterator(*const_cast<stream_wrapper_type*>(this), end_buffer_index);
        }

    private:
        stream_iterator_type m_end_iterator{};
        stream_iterator_type m_iterator;
        buffer_type m_buffer;

        buffer_type init_buffer(const stream_iterator_type& it, const stream_iterator_type& end) noexcept {
            buffer_type result;
            if (it != end) {
                result.push_back(*it);
            }
            return result;
        }

        friend const_iterator;
    };


} //namespace parserlib


#endif //PARSERLIB_STREAM_WRAPPER_HPP
