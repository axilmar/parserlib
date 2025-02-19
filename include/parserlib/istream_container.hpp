#ifndef PARSERLIB_ISTREAM_CONTAINER_HPP
#define PARSERLIB_ISTREAM_CONTAINER_HPP


#include <cstddef>
#include <istream>
#include <iterator>
#include <vector>
#include <cassert>


namespace parserlib {


    template <class Elem, class Traits>
    class istream_container {
    public:
        using istream_container_type = istream_container<Elem, Traits>;
        using type = istream_container_type;
        using stream_type = std::basic_istream<Elem, Traits>;
        using stream_iterator_type = std::istream_iterator<Elem, Elem, Traits>;
        using buffer_type = std::vector<Elem>;
        using value_type = Elem;

        class const_iterator {
        public:
            value_type operator *() const noexcept {
                assert(m_buffer_index < m_container.m_buffer.size());
                return m_container.m_buffer[m_buffer_index];
            }

            const_iterator& operator ++() noexcept {
                assert(m_buffer_index <= m_container.m_buffer.size());
                if (m_buffer_index < m_container.m_buffer.size()) {
                    ++m_buffer_index;
                }
                if (m_buffer_index == m_container.m_buffer.size()) {
                    ++m_container.m_iterator;
                    if (m_container.m_iterator != m_container.m_end_iterator) {
                        m_container.m_buffer.push_back(*m_container.m_iterator);
                    }
                }
                return *this;
            }

            bool operator == (const const_iterator& it) const noexcept {
                assert(&m_container == &it.m_container);
                return m_buffer_index == it.m_buffer_index ||
                       it.m_buffer_index == -1 && m_buffer_index == m_container.m_buffer.size() && m_container.m_iterator == m_container.m_end_iterator;
            }

            bool operator != (const const_iterator& it) const noexcept {
                return !operator == (it);
            }

            const istream_container_type& get() const noexcept {
                return *this;
            }

            istream_container_type& get() noexcept {
                return *this;
            }

        private:
            istream_container_type& m_container;
            std::size_t m_buffer_index;

            const_iterator(istream_container_type& container, std::size_t buffer_index) noexcept
                : m_container(container)
                , m_buffer_index(buffer_index)
            {
            }

            friend istream_container_type;
        };

        istream_container(std::basic_istream<Elem, Traits>& stream) noexcept
            : m_iterator(stream)
            , m_buffer(init_buffer(m_iterator, m_end_iterator))
        {            
        }

        const_iterator begin() const noexcept {
            return const_iterator(*const_cast<istream_container_type*>(this), 0);
        }

        const_iterator end() const noexcept {
            return const_iterator(*const_cast<istream_container_type*>(this), -1);
        }

    private:
        stream_iterator_type m_end_iterator{};
        stream_iterator_type m_iterator;
        buffer_type m_buffer;

        static std::vector<Elem> init_buffer(const stream_iterator_type& it, const stream_iterator_type& end) noexcept {
            buffer_type result;
            if (it != end) {
                result.push_back(*it);
            }
            return result;
        }

        friend const_iterator;
    };


} //namespace parserlib


#endif //PARSERLIB_ISTREAM_CONTAINER_HPP
