#ifndef PARSERLIB_SOURCESTRING_HPP
#define PARSERLIB_SOURCESTRING_HPP


#include <ostream>
#include "util.hpp"


namespace parserlib {


    /**
     * Traits to use for case-sensitive parsing.
     */
    class CaseSensitiveTraits {
    public:
        /**
         * Converts the given character to lower case.
         * In this call, no case lowering happens, in order to enable case-sensitive parsing.
         * @param a character to modify.
         * @return the given character, unmodifed.
         */
        template <class Char>
        static Char toLowerCase(const Char& a) {
            return a;
        }
    };


    /**
     * Traits to use for case-insensitive parsing.
     */
    class CaseInsensitiveTraits {
    public:
        /**
         * Converts the given character to lower case.
         * @return the given character to lower case.
         */
        template <class Char>
        static Char toLowerCase(const Char& a) {
            return parserlib::toLowerCase(a);
        }
    };


    /**
     * The default newline traits.
     */
    class DefaultNewlineTraits {
    public:
        /**
         * Given the current position and end position,
         * it returns the number of characters to increment the current position by,
         * in order to skip the newline sequence.
         * This implementation recognizes only the character `\n`.
         * @return 1 if the current character is `\n`, 0 otherwise.
         */
        template <class It>
        static size_t getNewlineIncrement(const It& it, const It& end) {
            return *it == '\n' ? 1 : 0;
        }
    };


    /**
     * A source string.
     * 
     * It provides a special iterator that counts lines and columns.
     * 
     * @param Source source string type; any STL-like container.
     * @param CaseTraits character case traits.
     * @param NewlineTraits newline traits.
     */
    template <class Source = std::string, class CaseTraits = CaseSensitiveTraits, class NewlineTraits = DefaultNewlineTraits> class SourceString {
    public:
        /**
         * The source type.
         */
        typedef Source Source;

        /**
         * The case traits type.
         */
        typedef CaseTraits CaseTraits;

        /**
         * The newline traits type.
         */
        typedef NewlineTraits NewlineTraits;

        /**
         * The iterator into the source.
         */
        typedef typename Source::const_iterator Iterator;

        /**
         * The value type, i.e. the character type.
         */
        typedef typename Source::value_type CharValue;

        /**
         * Wrapper around a character.
         * It provides the functions of comparison to the source.
         */
        class Char {
        public:
            /**
             * The constructor.
             * @param v the character value.
             */
            Char(const CharValue& v)
                : m_value(v)
            {
            }

            /**
             * Checks if this character equals the given one,
             * taking into account the case traits.
             * @param v the value to compare this with.
             * @return true if the characters are equal, false otherwise.
             */
            bool operator == (const CharValue& v) const {
                return CaseTraits::toLowerCase(m_value) == CaseTraits::toLowerCase(v);
            }

            /**
             * Checks if this character differs to the given one,
             * taking into account the case traits.
             * @param v the value to compare this with.
             * @return true if the characters are different, false otherwise.
             */
            bool operator != (const CharValue& v) const {
                return CaseTraits::toLowerCase(m_value) != CaseTraits::toLowerCase(v);
            }

            /**
             * Checks if this character is less than the given one,
             * taking into account the case traits.
             * @param v the value to compare this with.
             * @return true if this character is less than the given one, false otherwise.
             */
            bool operator < (const CharValue& v) const {
                return CaseTraits::toLowerCase(m_value) < CaseTraits::toLowerCase(v);
            }

            /**
             * Checks if this character is less than or equal to the given one,
             * taking into account the case traits.
             * @param v the value to compare this with.
             * @return true if this character is less than or equal to the given one, false otherwise.
             */
            bool operator <= (const CharValue& v) const {
                return CaseTraits::toLowerCase(m_value) <= CaseTraits::toLowerCase(v);
            }

            /**
             * Checks if this character is greater than the given one,
             * taking into account the case traits.
             * @param v the value to compare this with.
             * @return true if this character is greater than the given one, false otherwise.
             */
            bool operator > (const CharValue& v) const {
                return CaseTraits::toLowerCase(m_value) > CaseTraits::toLowerCase(v);
            }

            /**
             * Checks if this character is greater than or equal to the given one,
             * taking into account the case traits.
             * @param v the value to compare this with.
             * @return true if this character is greater than or equal to the given one, false otherwise.
             */
            bool operator >= (const CharValue& v) const {
                return CaseTraits::toLowerCase(m_value) >= CaseTraits::toLowerCase(v);
            }

            /**
             * Checks if the given character value equals the given character.
             * @param v the value to check.
             * @param c the character to check.
             * @return true if the value equals the character, false otherwise.
             */
            friend bool operator == (const CharValue& v, const Char& c) {
                return CaseTraits::toLowerCase(v) == CaseTraits::toLowerCase(c.m_value);
            }

            /**
             * Checks if the given character value differs from the given character.
             * @param v the value to check.
             * @param c the character to check.
             * @return true if the value is different than the character, false otherwise.
             */
            friend bool operator != (const CharValue& v, const Char& c) {
                return CaseTraits::toLowerCase(v) != CaseTraits::toLowerCase(c.m_value);
            }

            /**
             * Checks if the given character value is less than the given character.
             * @param v the value to check.
             * @param c the character to check.
             * @return true if the value is less than the character, false otherwise.
             */
            friend bool operator < (const CharValue& v, const Char& c) {
                return CaseTraits::toLowerCase(v) < CaseTraits::toLowerCase(c.m_value);
            }

            /**
             * Checks if the given character value is less than or equal to the given character.
             * @param v the value to check.
             * @param c the character to check.
             * @return true if the value is less than or equal to the character, false otherwise.
             */
            friend bool operator <= (const CharValue& v, const Char& c) {
                return CaseTraits::toLowerCase(v) <= CaseTraits::toLowerCase(c.m_value);
            }

            /**
             * Checks if the given character value is greater than the given character.
             * @param v the value to check.
             * @param c the character to check.
             * @return true if the value is greater than the character, false otherwise.
             */
            friend bool operator > (const CharValue& v, const Char& c) {
                return CaseTraits::toLowerCase(v) > CaseTraits::toLowerCase(c.m_value);
            }

            /**
             * Checks if the given character value is greater than or equal to the given character.
             * @param v the value to check.
             * @param c the character to check.
             * @return true if the value is greater than or equal to the character, false otherwise.
             */
            friend bool operator >= (const CharValue& v, const Char& c) {
                return CaseTraits::toLowerCase(v) >= CaseTraits::toLowerCase(c.m_value);
            }

        private:
            const CharValue m_value;
        };

        /**
         * An iterator into the source that knows how to count lines and columns,
         * using the provided newline traits.
         */
        class const_iterator {
        public:
            /**
             * The default constructor.
             * An empty iterator is created.
             */
            const_iterator() {
            }

            /**
             * A constructor from parameters.
             * @param it iterator into the source.
             * @param line current line.
             * @param column current column.
             * @param end the end position of the source; required for newline sequences that are 
             *  bigger than a single character.
             */
            const_iterator(const Iterator& it, size_t line, size_t column, const Iterator& end)
                : m_iterator(it)
                , m_line(line)
                , m_column(column)
                , m_end(end)
            {
            }

            /**
             * Returns the iterator to source.
             * @return the iterator to source.
             */
            const Iterator& getIterator() const {
                return m_iterator;
            }

            /**
             * Automatic conversion to iterator to source.
             * @return the iterator to source.
             */
            operator const Iterator& () const {
                return getIterator();
            }

            /**
             * Returns the current line.
             * @return the current line.
             */
            size_t getLine() const {
                return m_line;
            }

            /**
             * Returns the column.
             * @return the column.
             */
            size_t getColumn() const {
                return m_column;
            }

            /**
             * Returns the character at the iterator.
             * @return the character at the iterator.
             */
            Char operator *() const {
                return *m_iterator;
            }

            /**
             * Increments the iterator.
             * It increments the line/column values depending on the newline traits.
             * @return reference to this.
             */
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

            /**
             * Advances the iterator multiple positions.
             * @param size number of positions to increment the iterator.
             */
            const_iterator operator += (size_t size) {
                for (; size > 0; --size) {
                    operator ++();
                }
                return *this;
            }

            /**
             * Checks if this and the given iterator are equal.
             * @param right the other iterator to check.
             * @return true if this and the given iterator are equal, false otherwise.
             */
            bool operator == (const const_iterator& right) const {
                return m_iterator == right.m_iterator;
            }

            /**
             * Checks if this and the given iterator are different.
             * @param right the other iterator to check.
             * @return true if this and the given iterator are different, false otherwise.
             */
            bool operator != (const const_iterator& right) const {
                return m_iterator != right.m_iterator;
            }

            /**
             * Checks if this is less than the given iterator.
             * @param right the other iterator to check.
             * @return true if this is less than the given iterator, false otherwise.
             */
            bool operator < (const const_iterator& right) const {
                return m_iterator < right.m_iterator;
            }

            /**
             * Checks if this is less than or equal to the given iterator.
             * @param right the other iterator to check.
             * @return true if this is less than or equal to the given iterator, false otherwise.
             */
            bool operator <= (const const_iterator& right) const {
                return m_iterator <= right.m_iterator;
            }

            /**
             * Checks if this is greater than the given iterator.
             * @param right the other iterator to check.
             * @return true if this is greater than the given iterator, false otherwise.
             */
            bool operator > (const const_iterator& right) const {
                return m_iterator > right.m_iterator;
            }

            /**
             * Checks if this is greater than or equal to the given iterator.
             * @param right the other iterator to check.
             * @return true if this is greater than or equal to the given iterator, false otherwise.
             */
            bool operator >= (const const_iterator& right) const {
                return m_iterator >= right.m_iterator;
            }

            /**
             * Checks if this and the given iterator are equal.
             * @param right the other iterator to check.
             * @return true if this and the given iterator are equal, false otherwise.
             */
            bool operator == (const Iterator& right) const {
                return m_iterator == right;
            }

            /**
             * Checks if this and the given iterator are different.
             * @param right the other iterator to check.
             * @return true if this and the given iterator are different, false otherwise.
             */
            bool operator != (const Iterator& right) const {
                return m_iterator != right;
            }

            /**
             * Checks if this is less than the given iterator.
             * @param right the other iterator to check.
             * @return true if this is less than the given iterator, false otherwise.
             */
            bool operator < (const Iterator& right) const {
                return m_iterator < right;
            }

            /**
             * Checks if this is less than or equal to the given iterator.
             * @param right the other iterator to check.
             * @return true if this is less than or equal to the given iterator, false otherwise.
             */
            bool operator <= (const Iterator& right) const {
                return m_iterator <= right;
            }

            /**
             * Checks if this is greater than the given iterator.
             * @param right the other iterator to check.
             * @return true if this is greater than the given iterator, false otherwise.
             */
            bool operator > (const Iterator& right) const {
                return m_iterator > right;
            }

            /**
             * Checks if this is greater than or equal to the given iterator.
             * @param right the other iterator to check.
             * @return true if this is greater than or equal to the given iterator, false otherwise.
             */
            bool operator >= (const Iterator& right) const {
                return m_iterator >= right;
            }

            /**
             * Checks if the left and the right iterator are equal.
             * @param left the left iterator to check.
             * @param right the right iterator to check.
             * @return true if the left and the right iterator are equal, false otherwise.
             */
            friend bool operator == (const Iterator& left, const const_iterator& right) {
                return left == right.getIterator();
            }

            /**
             * Checks if the left and the right iterator are different.
             * @param left the left iterator to check.
             * @param right the right iterator to check.
             * @return true if the left and the right iterator are different, false otherwise.
             */
            friend bool operator != (const Iterator& left, const const_iterator& right) {
                return left != right.getIterator();
            }

            /**
             * Checks if the left is less than the right iterator.
             * @param left the left iterator to check.
             * @param right the right iterator to check.
             * @return true if the left is less than the right iterator, false otherwise.
             */
            friend bool operator < (const Iterator& left, const const_iterator& right) {
                return left < right.getIterator();
            }

            /**
             * Checks if the left is less than or equal to the right iterator.
             * @param left the left iterator to check.
             * @param right the right iterator to check.
             * @return true if the left is less than or equal to the right iterator, false otherwise.
             */
            friend bool operator <= (const Iterator& left, const const_iterator& right) {
                return left <= right.getIterator();
            }

            /**
             * Checks if the left is greater than the right iterator.
             * @param left the left iterator to check.
             * @param right the right iterator to check.
             * @return true if the left is greater than the right iterator, false otherwise.
             */
            friend bool operator > (const Iterator& left, const const_iterator& right) {
                return left > right.getIterator();
            }

            /**
             * Checks if the left is greater than or equal to the right iterator.
             * @param left the left iterator to check.
             * @param right the right iterator to check.
             * @return true if the left is greater than or equal to the right iterator, false otherwise.
             */
            friend bool operator >= (const Iterator& left, const const_iterator& right) {
                return left >= right.getIterator();
            }

            /**
             * Computes the delta between two iterators.
             * @param left the first iterator.
             * @param right the right iterator.
             * @return the delta between the iterators.
             */
            friend size_t operator - (const const_iterator& left, const const_iterator& right) {
                return left.getIterator() - right.getIterator();
            }

        private:
            Iterator m_iterator;
            Iterator m_end;
            size_t m_line;
            size_t m_column;
        };

        /**
         * Constructor from source.
         * @param source the source; copied internally to another instance of Source.
         */
        SourceString(const Source& source)
            : m_source(source)
        {
        }

        /**
         * Constructor from null-terminated source string.
         * @param source the null-terminated string value; copied internally to an instance of Source.
         */
        SourceString(const CharValue* source)
            : m_source(source)
        {
        }

        /**
         * Constructor from source range.
         * @param begin beginning of range.
         * @param end end of range.
         */
        SourceString(const const_iterator& begin, const const_iterator& end)
            : m_source(begin.getIterator(), end.getIterator())
        {
        }

        /**
         * Produces an iterator that starts from position 0, 0 into the source.
         * @return an iterator to the start of the source.
         */
        const_iterator begin() const {
            return const_iterator(m_source.begin(), 0, 0, m_source.end());
        }

        /**
         * Produces an iterator that that points to the end of the source.
         * Position is set to max value for size_t, for line and column.
         * @return an iterator to the end of the source.
         */
        const_iterator end() const {
            return const_iterator(m_source.end(), -1, -1, m_source.end());
        }

        /**
         * Returns the underlying source.
         * @return the underlying source.
         */
        const Source& getSource() const {
            return m_source;
        }

        /**
         * Auto conversion to the underying source.
         * @return the underlying source.
         */
        operator const Source& () const {
            return getSource();
        }

        /**
         * Checks if this is equal to the given source string.
         * @param right the other source string to check.
         * @return true if this is equal to the given source string, false otherwise.
         */
        bool operator == (const SourceString& right) const {
            return m_source == right.m_source;
        }

        /**
         * Checks if this is different to the given source string.
         * @param right the other source string to check.
         * @return true if this is different to the given source string, false otherwise.
         */
        bool operator != (const SourceString& right) const {
            return m_source != right.m_source;
        }

        /**
         * Checks if this is less than the given source string.
         * @param right the other source string to check.
         * @return true if this is less than the given source string, false otherwise.
         */
        bool operator < (const SourceString& right) const {
            return m_source < right.m_source;
        }

        /**
         * Checks if this is less than or equal to the given source string.
         * @param right the other source string to check.
         * @return true if this is less than or equal to the given source string, false otherwise.
         */
        bool operator <= (const SourceString& right) const {
            return m_source <= right.m_source;
        }

        /**
         * Checks if this is greater than the given source string.
         * @param right the other source string to check.
         * @return true if this is greater than the given source string, false otherwise.
         */
        bool operator > (const SourceString& right) const {
            return m_source > right.m_source;
        }

        /**
         * Checks if this is greater than or equal to the given source string.
         * @param right the other source string to check.
         * @return true if this is greater than or equal to the given source string, false otherwise.
         */
        bool operator >= (const SourceString& right) const {
            return m_source >= right.m_source;
        }

    private:
        Source m_source;
    };


    /**
     * Sends the given source string to the given output stream.
     * @param stream stream to output to.
     * @param src source string to output.
     * @return the output stream.
     */
    template <class Elem, class Traits, class Source>
    std::basic_ostream<Elem, Traits>& operator << (std::basic_ostream<Elem, Traits>& stream, const SourceString<Source>& src) {
        stream << src.getSource();
        return stream;
    }


} //namespace parserlib


#endif //PARSERLIB_SOURCESTRING_HPP
