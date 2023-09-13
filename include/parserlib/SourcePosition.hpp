#ifndef PARSERLIB_SOURCEPOSITION_HPP
#define PARSERLIB_SOURCEPOSITION_HPP


#include <cctype>
#include <vector>
#include <string>


namespace parserlib {


    /**
     * The default implementation of source position.
     * @param SourceType source type.
     * @param CaseSensitive if true, comparison is case sensitive, otherwise case insensitive.
     */
    template <class SourceType = std::string, bool CaseSensitive = true> class SourcePosition {
    public:
        /**
         * The default constructor.
         */
        SourcePosition() {
        }

        /**
         * Constructor.
         * @param begin iterator to the first element of the source.
         * @param end iterator to the end of the source.
         */
        SourcePosition(const typename SourceType::const_iterator& begin, const typename SourceType::const_iterator& end)
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
         * @param iterator position in source that contains the element to compare to the value.
         * @param value value to compare with the value at the current position.
         * @return true if equal, false otherwise.
         */
        template <class T>
        static bool contains(const typename SourceType::const_iterator& iterator, const T& value) {
            if constexpr (CaseSensitive) {
                return *iterator == value;
            }
            else {
                return std::tolower(*iterator) == std::tolower(value);
            }
        }

        /**
         * Compares the current value with the given range of v alues.
         * If CaseSensitive is false, then values are set to lowercase before compared.
         * @param iterator position in source that contains the element to compare to the value.
         * @param minValue lowest value to compare with the value at the current position.
         * @param maxValue max value to compare with the value at the current position.
         * @return true if within range, false otherwise.
         */
        template <class T>
        static bool contains(const typename SourceType::const_iterator& iterator, const T& minValue, const T& maxValue) {
            if constexpr (CaseSensitive) {
                return *iterator >= minValue && *iterator <= maxValue;
            }
            else {
                return std::tolower(*iterator) >= std::tolower(minValue) && std::tolower(*iterator) <= std::tolower(maxValue);
            }
        }

        /**
         * Compares the current value with the given array of values.
         * If CaseSensitive is false, then values are set to lowercase before compared.
         * @param iterator position in source that contains the element to compare to the value.
         * @param values.
         * @return true if within container, false otherwise.
         */
        template <class T, class Alloc>
        static bool contains(const typename SourceType::const_iterator& iterator, const std::vector<T, Alloc>& values) {
            for (const T& value : values) {
                if (contains(iterator, value)) {
                    return true;
                }
            }
            return false;
        }

        /**
         * Compares the current value with the given null-terminated string.
         * If CaseSensitive is false, then values are set to lowercase before compared.
         * @param iterator position in source that contains the element to compare to the value.
         * @param str null-terminated string.
         * @return true if string is present at the given position, false otherwise.
         */
        template <class T>
        static bool contains(const typename SourceType::const_iterator& iterator, const typename SourceType::const_iterator& end, const T* str) {
            auto it = iterator;
            const T* ts = str;

            for (;;) {
                //if string exhausted, then it was recognized successfully
                if (!*ts) {
                    break;
                }

                //if iteration reached the end of source, the string was not recognized
                if (it == end) {
                    return false;
                }

                //if the characters differ, then the string is not recognized
                if (!contains(it, *ts)) {
                    return false;
                }

                //next character
                ++it;
                ++ts;
            }

            //success
            return true;
        }

        /**
         * Compares the current value with the given one.
         * If CaseSensitive is false, then both values are set to lowercase before compared.
         * @param value value to compare with the value at the current position.
         * @return true if equal, false otherwise.
         */
        template <class T>
        bool contains(const T& value) const {
            return contains(m_iterator, value);
        }

        /**
         * Compares the current value with the given range of values.
         * If CaseSensitive is false, then values are set to lowercase before compared.
         * @param minValue lowest value to compare with the value at the current position.
         * @param maxValue max value to compare with the value at the current position.
         * @return true if within range, false otherwise.
         */
        template <class T>
        bool contains(const T& minValue, const T& maxValue) const {
            return contains(m_iterator, minValue, maxValue);
        }

        /**
         * Compares the current value with the given array of values.
         * If CaseSensitive is false, then values are set to lowercase before compared.
         * @param values.
         * @return true if within range, false otherwise.
         */
        template <class T, class Alloc>
        bool contains(const std::vector<T, Alloc>& values) const {
            return contains(m_iterator, values);
        }

        /**
         * Compares the current value with the given null-terminated string.
         * If CaseSensitive is false, then values are set to lowercase before compared.
         * @param str null-terminated string.
         * @return true if string is present at the given position, false otherwise.
         */
        template <class T>
        bool contains(const T* str) const {
            return contains(m_iterator, m_end, str);
        }

        /**
         * Increments the position by one place.
         */
        void increment() {
            ++m_iterator;
        }

        /**
         * Increases the position by multiple places.
         * @param count number of places to increase the position by.
         */
        void increase(size_t count) {
            m_iterator += count;
        }

        /**
         * Checks if the two positions are equal.
         * @param other the other position to compare this to.
         * @return true if they are equal, false otherwise.
         */
        bool operator == (const SourcePosition& other) const {
            return m_iterator == other.m_iterator;
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
         * Checks if the two positions are different.
         * @param other the other position to compare this to.
         * @return true if they are different, false otherwise.
         */
        bool operator != (const SourcePosition& other) const {
            return m_iterator != other.m_iterator;
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
         * Checks if the this position comes before the other position.
         * @param other the other position to compare this to.
         * @return true if the comparison is true, false otherwise.
         */
        bool operator < (const SourcePosition& other) const {
            return m_iterator < other.m_iterator;
        }

        /**
         * Checks if this position's iterator comes before the given iterator.
         * @param it iterator to compare to this.
         * @return true if the comparison is true, false otherwise.
         */
        bool operator < (const typename SourceType::const_iterator& it) const {
            return m_iterator < it;
        }

        /**
         * Checks if the this position comes after the other position.
         * @param other the other position to compare this to.
         * @return true if the comparison is true, false otherwise.
         */
        bool operator > (const SourcePosition& other) const {
            return m_iterator > other.m_iterator;
        }

        /**
         * Checks if this position's iterator comes after the given iterator.
         * @param it iterator to compare to this.
         * @return true if the comparison is true, false otherwise.
         */
        bool operator > (const typename SourceType::const_iterator& it) const {
            return m_iterator > it;
        }

        /**
         * Checks if the this position comes after or is equal to the other position.
         * @param other the other position to compare this to.
         * @return true if the comparison is true, false otherwise.
         */
        bool operator <= (const SourcePosition& other) const {
            return m_iterator <= other.m_iterator;
        }

        /**
         * Checks if this position's iterator comes before or is equal to the given iterator.
         * @param it iterator to compare to this.
         * @return true if the comparison is true, false otherwise.
         */
        bool operator <= (const typename SourceType::const_iterator& it) const {
            return m_iterator <= it;
        }

        /**
         * Checks if the this position comes after or is equal to the other position.
         * @param other the other position to compare this to.
         * @return true if the comparison is true, false otherwise.
         */
        bool operator >= (const SourcePosition& other) const {
            return m_iterator >= other.m_iterator;
        }

        /**
         * Checks if this position's iterator comes after or is equal to the given iterator.
         * @param it iterator to compare to this.
         * @return true if the comparison is true, false otherwise.
         */
        bool operator >= (const typename SourceType::const_iterator& it) const {
            return m_iterator >= it;
        }

    private:
        typename SourceType::const_iterator m_iterator;
        typename SourceType::const_iterator m_end;
    };


} //namespace parserlib


#endif //PARSERLIB_SOURCEPOSITION_HPP
