#ifndef PARSERLIB_LINECOUNTINGSOURCEPOSITION_HPP
#define PARSERLIB_LINECOUNTINGSOURCEPOSITION_HPP


#include <cctype>


namespace parserlib {


    /**
     * Default newline traits.
     */
    class DefaultNewlineTraits {
    public:
        /**
         * Checks if the character at the given position is '\n'.
         * @param it iterator to the source.
         * @param end iterator to the end of the source.
         * @return true if the character is '\n', false otherwise.
         */
        template <class Iterator> bool isNewline(const Iterator& it, const Iterator& end) const {
            return *it == '\n';
        }

        /**
         * Increments the iterator in order to skip the newline.
         * @param it iterator to the source.
         * @param end iterator to the end of the source.
         */
        template <class Iterator> void skipNewline(Iterator& it, const Iterator& end) const {
            ++it;
        }
    };


    /**
     * A source position that counts lines and columns.
     * @param SourceType source type.
     * @param CaseSensitive if true, comparison is case sensitive, otherwise case insensitive.
     * @param NewlineTraits traits for newline.
     */
    template <class SourceType, bool CaseSensitive = true, class NewlineTraits = DefaultNewlineTraits> class LineCountingSourcePosition {
    public:
        /**
         * The default constructor.
         */
        LineCountingSourcePosition() {
        }

        /**
         * Constructor.
         * @param begin iterator to the first element of the source.
         * @param end iterator to the end of the source.
         */
        LineCountingSourcePosition(const typename SourceType::const_iterator& begin, const typename SourceType::const_iterator& end)
            : m_iterator(begin)
            , m_end(end)
        {
        }

        /**
         * Returns the iterator.
         * @return the iterator.
         */
        const typename SourceType::const_iterator& iterator() const {
            return m_iterator;
        }

        /**
         * Returns the end of the source.
         * @return the end of the source.
         */
        const typename SourceType::const_iterator& end() const {
            return m_end;
        }

        /**
         * Compares the current value with the given one.
         * If CaseSensitive is false, then both values are set to lowercase before compared.
         * @param value value to compare with the value at the current position.
         * @return true if equal, false otherwise.
         */
        bool contains(const typename SourceType::value_type& value) const {
            if constexpr (CaseSensitive) {
                return *m_iterator == value;
            }
            else {
                return std::tolower(*m_iterator) == std::tolower(value);
            }
        }

        /**
         * Compares the current value with the given range of v alues.
         * If CaseSensitive is false, then values are set to lowercase before compared.
         * @param minValue lowest value to compare with the value at the current position.
         * @param maxValue max value to compare with the value at the current position.
         * @return true if within range, false otherwise.
         */
        bool contains(const typename SourceType::value_type& minValue, const typename SourceType::value_type& maxValue) const {
            if constexpr (CaseSensitive) {
                return *m_iterator >= minValue && *m_iterator <= maxValue;
            }
            else {
                return std::tolower(*m_iterator) >= std::tolower(minValue) && std::tolower(*m_iterator) <= std::tolower(maxValue);
            }
        }

        /**
         * Increments the position by one place.
         */
        void increment() {
            if (NewlineTraits().isNewline(m_iterator, m_end)) {
                ++m_line;
                m_column = 1;
            }
            else {
                ++m_column;
            }
            ++m_iterator;
        }

        /**
         * Increases the position by multiple places.
         * @param count number of places to increase the position by.
         */
        void increase(size_t count) {
            m_column += count;
            m_iterator += count;
        }

        /**
         * Checks if the two positions are equal.
         * @param other the other position to compare this to.
         * @return true if they are equal, false otherwise.
         */
        bool operator == (const LineCountingSourcePosition& other) const {
            return m_iterator == other.m_iterator;
        }

        /**
         * Checks if the two positions are different.
         * @param other the other position to compare this to.
         * @return true if they are different, false otherwise.
         */
        bool operator != (const LineCountingSourcePosition& other) const {
            return m_iterator != other.m_iterator;
        }

        /**
         * Checks if this position is equal to the given iterator.
         * @param it iterator to compare to this.
         * @return true if they are equal, false otherwise.
         */
        bool operator == (const typename SourceType::const_iterator& it) const {
            return m_iterator == it;
        }

        /**
         * Checks if this position is different to the given iterator.
         * @param it iterator to compare to this.
         * @return true if they are different, false otherwise.
         */
        bool operator != (const typename SourceType::const_iterator& it) const {
            return m_iterator != it;
        }

        /**
         * Returns the line.
         * @return the line.
         */
        size_t line() const {
            return m_line;
        }

        /**
         * Returns the column.
         * @return the column.
         */
        size_t column() const {
            return m_column;
        }

    private:
        typename SourceType::const_iterator m_iterator;
        typename SourceType::const_iterator m_end;
        size_t m_line{1};
        size_t m_column{1};
    };


} //namespace parserlib


#endif //PARSERLIB_LINECOUNTINGSOURCEPOSITION_HPP
