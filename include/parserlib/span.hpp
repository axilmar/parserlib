#ifndef PARSERLIB_SPAN_HPP
#define PARSERLIB_SPAN_HPP


#include <cassert>
#include <algorithm>
#include <type_traits>
#include <vector>
#include <string_view>


namespace parserlib {


    /**
     * A class that represents an view of a part of a container.
     * @param Iterator iterator type.
     */
    template <class Iterator> 
    class span {
    public:
        /** Iterator type. */
        using iterator_type = Iterator;

        /** Input token type. */
        using input_token_type = typename iterator_type::value_type;

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
        span(const Iterator& begin, const Iterator& end)
            : m_begin(begin)
            , m_end(end)
        {
            assert(begin <= end);
        }

        /**
         * Returns the begin iterator.
         * @return the begin iterator.
         */
        const Iterator& cbegin() const {
            return m_begin;
        }

        /**
         * Returns the end iterator.
         * @return the end iterator.
         */
        const Iterator& cend() const {
            return m_end;
        }

        /**
         * Returns the begin iterator.
         * @return the begin iterator.
         */
        const Iterator& begin() const {
            return m_begin;
        }

        /**
         * Returns the end iterator.
         * @return the end iterator.
         */
        const Iterator& end() const {
            return m_end;
        }

        /**
         * Returns the distance between the begin and end iterators.
         * @return the distance between the begin and end iterators.
         */
        auto distance() const {
            return std::distance(m_begin, m_end);
        }

        /**
         * Returns a pointer to the data.
         * It assumes that the begin iterator points to contiguous space.
         * @return a pointer to the data.
         */
        auto data() const {
            return &*begin();
        }

        /**
         * Returns the number of the elements.
         * Same as distance.
         * @return the number of the elements.
         */
        auto size() const {
            return distance();
        }

        /**
         * Returns the source that this span corresponds to.
         * If the input token is trivial, then it returns an std::basic_string_view,
         * otherwise it returns a vector that is copied from the source.
         * @return the source that this node corresponds to.
         */
        auto source() const {
            if constexpr (std::is_trivial_v<input_token_type>) {
                return std::basic_string_view<input_token_type>(data(), size());
            }
            else {
                return std::vector<input_token_type>(m_begin, m_end);
            }
        }

    private:
        Iterator m_begin;
        Iterator m_end;
    };


} //namespace parserlib


#endif //PARSERLIB_SPAN_HPP
