#ifndef PARSERLIB_STRING_WRAPPER_HPP
#define PARSERLIB_STRING_WRAPPER_HPP


#include <cstddef>
#include <string>
#include <utility>


namespace parserlib {


    class DefaultNewlineSequenceLength {
    public:
        template <class It>
        std::size_t operator ()(const It& begin, const It& end) const noexcept {
            return *begin == '\n';
        }
    };


    template <class String, class StringStorage = String&, class NewlineSequenceLength = DefaultNewlineSequenceLength>
    class string_wrapper {
    public:
        using string_wrapper_type = string_wrapper<String, StringStorage, NewlineSequenceLength>;
        
        using string_type = String;
        using newline_sequence_length_type = NewlineSequenceLength;
        using string_storage_type = StringStorage;

        using value_type = typename String::value_type;
        using string_iterator_type = typename String::const_iterator;

        class const_iterator {
        public:
            const_iterator()
                : m_string(nullptr)
                , m_iterator{}
                , m_line(-1)
                , m_column(-1)
            {
            }

            value_type operator *() const noexcept {
                return *m_iterator;
            }

            const_iterator& operator ++() noexcept {
                const std::size_t newlineSequenceLength = NewlineSequenceLength()(m_iterator, m_string->end());
                if (newlineSequenceLength) {
                    m_iterator += newlineSequenceLength;
                    ++m_line;
                    m_column = 0;
                }
                else {
                    ++m_iterator;
                    ++m_column;
                }
                return *this;
            }

            bool operator == (const const_iterator& it) const noexcept {
                return m_iterator == it.m_iterator;
            }

            bool operator != (const const_iterator& it) const noexcept {
                return m_iterator != it.m_iterator;
            }

            bool operator < (const const_iterator& it) const noexcept {
                return m_iterator < it.m_iterator;
            }

            bool operator <= (const const_iterator& it) const noexcept {
                return m_iterator <= it.m_iterator;
            }

            bool operator > (const const_iterator& it) const noexcept {
                return m_iterator > it.m_iterator;
            }

            bool operator >= (const const_iterator& it) const noexcept {
                return m_iterator >= m_iterator;
            }

            std::size_t line() const {
                return m_line;
            }

            std::size_t column() const {
                return m_column;
            }

        private:
            const string_type* m_string;
            string_iterator_type m_iterator;
            std::size_t m_line;
            std::size_t m_column;

            const_iterator(const string_type& string, const string_iterator_type& it, std::size_t line, std::size_t column) noexcept
                : m_string(&string)
                , m_iterator(it)
                , m_line(line)
                , m_column(column)
            {
            }

            friend string_wrapper_type;
        };

        template <class T>
        string_wrapper(T&& string) noexcept
            : m_string(std::forward<T>(string))
        {
        }

        const_iterator begin() const noexcept {
            return const_iterator(m_string, m_string.begin(), 0, 0);
        }

        const_iterator end() const noexcept {
            return const_iterator(m_string, m_string.end(), -1, -1);
        }

        const string_type& string() const noexcept {
            return m_string;
        }

    private:
        string_storage_type m_string;
    };


} //namespace parserlib


#endif //PARSERLIB_STRING_WRAPPER_HPP
