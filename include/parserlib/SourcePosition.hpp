#ifndef PARSERLIB_SOURCEPOSITION_HPP
#define PARSERLIB_SOURCEPOSITION_HPP


#include <cctype>


namespace parserlib {


    /**
     * The default implementation of source position.
     * @param SourceType source type.
     * @param CaseSensitive if true, comparison is case sensitive, otherwise case insensitive.
     */
    template <class SourceType, bool CaseSensitive = true> class SourcePosition {
    public:
        /**
         * The default constructor.
         */
        SourcePosition() {
        }

        /**
         * Constructor.
         * @param iterator iterator to the first element of the source.
         */
        SourcePosition(const typename SourceType::const_iterator& iterator)
            : m_iterator(iterator)
        {
        }

        /**
         * Returns the iterator.
         * @return the iterator.
         */
        typename const SourceType::const_iterator& iterator() const {
            return m_iterator;
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
         * Checks if the two positions are different.
         * @param other the other position to compare this to.
         * @return true if they are different, false otherwise.
         */
        bool operator != (const SourcePosition& other) const {
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

    private:
        typename SourceType::const_iterator m_iterator;
    };


} //namespace parserlib


#endif //PARSERLIB_SOURCEPOSITION_HPP
