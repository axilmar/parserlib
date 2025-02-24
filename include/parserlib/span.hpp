#ifndef PARSERLIB_SPAN_HPP
#define PARSERLIB_SPAN_HPP


#include <cassert>
#include <algorithm>


namespace parserlib {


    /**
     * A class that represents an view of a part of a container.
     * (Also provided by c++20, but not in c++17).
     * @param Iterator iterator type.
     */
    template <class Iterator> 
    class span {
    public:
        /** Iterator type. */
        using iterator_type = Iterator;

        /**
         * The empty span constructor.
         */
        span()
            : m_begin()
            , m_end()
        {
        }

        /**
         * Constructor from range.
         * @param begin start of range.
         * @param end end of range.
         */
        span(const Iterator& begin, const Iterator& end) noexcept
            : m_begin(begin)
            , m_end(end)
        {
            assert(begin <= end);
        }

        /**
         * Returns the begin iterator.
         * @return the begin iterator.
         */
        const Iterator& cbegin() const noexcept {
            return m_begin;
        }

        /**
         * Returns the end iterator.
         * @return the end iterator.
         */
        const Iterator& cend() const noexcept {
            return m_end;
        }

        /**
         * Returns the begin iterator.
         * @return the begin iterator.
         */
        const Iterator& begin() const noexcept {
            return m_begin;
        }

        /**
         * Returns the end iterator.
         * @return the end iterator.
         */
        const Iterator& end() const noexcept {
            return m_end;
        }

        /**
         * Returns the distance between the begin and end iterators.
         * @return the distance between the begin and end iterators.
         */
        auto distance() const noexcept {
            return std::distance(m_begin, m_end);
        }

        /**
         * Returns a pointer to the data.
         * It assumes that the begin iterator points to contiguous space.
         * @return a pointer to the data.
         */
        auto data() const noexcept {
            return &*begin();
        }

        /**
         * Returns the number of the elements.
         * Same as distance.
         * @return the number of the elements.
         */
        auto size() const noexcept {
            return distance();
        }

    private:
        Iterator m_begin;
        Iterator m_end;
    };


} //namespace parserlib


#endif //PARSERLIB_SPAN_HPP
