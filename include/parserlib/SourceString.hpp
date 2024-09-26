#ifndef PARSERLIB_SOURCESTRING_HPP
#define PARSERLIB_SOURCESTRING_HPP


#include <ostream>
#include "util.hpp"


namespace parserlib {


    class CaseSensitiveTraits {
    public:
        template <class Char>
        static Char toLowerCase(const Char& a) {
            return a;
        }
    };


    class CaseInsensitiveTraits {
    public:
        template <class Char>
        static Char toLowerCase(const Char& a) {
            return parserlib::toLowerCase(a);
        }
    };


    class DefaultNewlineTraits {
    public:
        template <class It>
        static size_t getNewlineIncrement(const It& it, const It& end) {
            return *it == '\n' ? 1 : 0;
        }
    };


    template <class Source = std::string, class CaseTraits = CaseSensitiveTraits, class NewlineTraits = DefaultNewlineTraits> class SourceString {
    public:
        typedef Source Source;

        typedef CaseTraits CaseTraits;

        typedef NewlineTraits NewlineTraits;

        typedef typename Source::const_iterator SourceIterator;

        typedef typename Source::value_type value_type;

        class Char {
        public:
            Char(const value_type& v)
                : m_value(v)
            {
            }

            bool operator == (const value_type& v) const {
                return CaseTraits::toLowerCase(m_value) == CaseTraits::toLowerCase(v);
            }

            bool operator != (const value_type& v) const {
                return CaseTraits::toLowerCase(m_value) != CaseTraits::toLowerCase(v);
            }

            bool operator > (const value_type& v) const {
                return CaseTraits::toLowerCase(m_value) > CaseTraits::toLowerCase(v);
            }

            bool operator >= (const value_type& v) const {
                return CaseTraits::toLowerCase(m_value) >= CaseTraits::toLowerCase(v);
            }

            bool operator < (const value_type& v) const {
                return CaseTraits::toLowerCase(m_value) < CaseTraits::toLowerCase(v);
            }

            bool operator <= (const value_type& v) const {
                return CaseTraits::toLowerCase(m_value) <= CaseTraits::toLowerCase(v);
            }

            friend bool operator == (const value_type& v, const Char& c) {
                return CaseTraits::toLowerCase(v) == CaseTraits::toLowerCase(c.m_value);
            }

            friend bool operator != (const value_type& v, const Char& c) {
                return CaseTraits::toLowerCase(v) != CaseTraits::toLowerCase(c.m_value);
            }

            friend bool operator < (const value_type& v, const Char& c) {
                return CaseTraits::toLowerCase(v) < CaseTraits::toLowerCase(c.m_value);
            }

            friend bool operator <= (const value_type& v, const Char& c) {
                return CaseTraits::toLowerCase(v) <= CaseTraits::toLowerCase(c.m_value);
            }

            friend bool operator > (const value_type& v, const Char& c) {
                return CaseTraits::toLowerCase(v) > CaseTraits::toLowerCase(c.m_value);
            }

            friend bool operator >= (const value_type& v, const Char& c) {
                return CaseTraits::toLowerCase(v) >= CaseTraits::toLowerCase(c.m_value);
            }

        private:
            const value_type m_value;
        };

        class const_iterator {
        public:
            const_iterator() {
            }

            const_iterator(const SourceIterator& it, size_t line, size_t column, const SourceIterator& end)
                : m_iterator(it)
                , m_line(line)
                , m_column(column)
                , m_end(end)
            {
            }

            const SourceIterator& getIterator() const {
                return m_iterator;
            }

            operator const SourceIterator& () const {
                return getIterator();
            }

            size_t getLine() const {
                return m_line;
            }

            size_t getColumn() const {
                return m_column;
            }

            Char operator *() const {
                return *m_iterator;
            }

            const_iterator& operator ++() {
                size_t newlineIncrement = NewlineTraits::getNewlineIncrement(m_iterator, m_end);
                if (newlineIncrement) {
                    m_iterator += newlineIncrement;
                    ++m_line;
                    m_column = 0;
                }
                else {
                    ++m_column;
                    ++m_iterator;
                }
                return *this;
            }

            const_iterator operator += (size_t size) {
                for (; size > 0; --size) {
                    operator ++();
                }
                return *this;
            }

            bool operator == (const const_iterator& right) const {
                return m_iterator == right.m_iterator;
            }

            bool operator != (const const_iterator& right) const {
                return m_iterator != right.m_iterator;
            }

            bool operator < (const const_iterator& right) const {
                return m_iterator < right.m_iterator;
            }

            bool operator <= (const const_iterator& right) const {
                return m_iterator <= right.m_iterator;
            }

            bool operator > (const const_iterator& right) const {
                return m_iterator > right.m_iterator;
            }

            bool operator >= (const const_iterator& right) const {
                return m_iterator >= right.m_iterator;
            }

            bool operator == (const SourceIterator& right) const {
                return m_iterator == right;
            }

            bool operator != (const SourceIterator& right) const {
                return m_iterator != right;
            }

            bool operator < (const SourceIterator& right) const {
                return m_iterator < right;
            }

            bool operator <= (const SourceIterator& right) const {
                return m_iterator <= right;
            }

            bool operator > (const SourceIterator& right) const {
                return m_iterator > right;
            }

            bool operator >= (const SourceIterator& right) const {
                return m_iterator >= right;
            }

            friend bool operator == (const SourceIterator& left, const const_iterator& right) {
                return left == right.getIterator();
            }

            friend bool operator != (const SourceIterator& left, const const_iterator& right) {
                return left != right.getIterator();
            }

            friend bool operator < (const SourceIterator& left, const const_iterator& right) {
                return left < right.getIterator();
            }

            friend bool operator <= (const SourceIterator& left, const const_iterator& right) {
                return left <= right.getIterator();
            }

            friend bool operator > (const SourceIterator& left, const const_iterator& right) {
                return left > right.getIterator();
            }

            friend bool operator >= (const SourceIterator& left, const const_iterator& right) {
                return left >= right.getIterator();
            }

            friend const_iterator operator + (const const_iterator& a, size_t size) {
                return const_iterator(a.getIterator() + size, 0, 0, a.m_end);
            }

            friend SourceIterator operator - (const const_iterator& a, size_t size) {
                return const_iterator(a.getIterator() - size, 0, 0, a.m_end);
            }

            friend size_t operator - (const const_iterator& a, const const_iterator& b) {
                return a.getIterator() - b.getIterator();
            }

        private:
            SourceIterator m_iterator;
            SourceIterator m_end;
            size_t m_line;
            size_t m_column;
        };

        SourceString(const Source& source)
            : m_source(source)
        {
        }

        SourceString(const value_type* source)
            : m_source(source)
        {
        }

        SourceString(const const_iterator& begin, const const_iterator& end)
            : m_source(begin.getIterator(), end.getIterator())
        {
        }

        const_iterator begin() const {
            return const_iterator(m_source.begin(), 0, 0, m_source.end());
        }

        const_iterator end() const {
            return const_iterator(m_source.end(), -1, -1, m_source.end());
        }

        const Source& getSource() const {
            return m_source;
        }

        operator const Source& () const {
            return getSource();
        }

        bool operator == (const SourceString& right) const {
            return m_source == right.m_source;
        }

        bool operator != (const SourceString& right) const {
            return m_source != right.m_source;
        }

        bool operator < (const SourceString& right) const {
            return m_source < right.m_source;
        }

        bool operator <= (const SourceString& right) const {
            return m_source <= right.m_source;
        }

        bool operator > (const SourceString& right) const {
            return m_source > right.m_source;
        }

        bool operator >= (const SourceString& right) const {
            return m_source >= right.m_source;
        }

    private:
        Source m_source;
    };


    template <class Elem, class Traits, class Source>
    std::basic_ostream<Elem, Traits>& operator << (std::basic_ostream<Elem, Traits>& stream, const SourceString<Source>& src) {
        stream << src.getSource();
        return stream;
    }


} //namespace parserlib


#endif //PARSERLIB_SOURCESTRING_HPP
