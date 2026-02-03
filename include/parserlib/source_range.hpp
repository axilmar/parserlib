#ifndef PARSERLIB_SOURCE_RANGE_HPP
#define PARSERLIB_SOURCE_RANGE_HPP


#include <string>
#include <vector>
#include <type_traits>
#include <algorithm>
#include "is_char.hpp"


namespace parserlib {


    /**
     * A class that represents a source range.
     * @param Id id type.
     * @param Iterator iterator type.
     */
    template <class Id = int, class Iterator = typename std::string::const_iterator>
    class source_range {
    public:
        /** Id type. */
        using id_type = Id;

        /** Iterator type. */
        using iterator_type = Iterator;

        /** Source range type. */
        using source_range_type = source_range<Id, Iterator>;

        /** value type. */
        using value_type = typename iterator_type::value_type;

        /**
         * Returns the id of the source range.
         * @return the id of the source range.
         */
        const id_type& get_id() const noexcept {
            return m_id;
        }

        /**
         * Returns the begin iterator.
         * @return the begin iterator.
         */
        const iterator_type& begin() const noexcept {
            return m_begin;
        }

        /**
         * Returns the end iterator.
         * @return the end iterator.
         */
        const iterator_type& end() const noexcept {
            return m_end;
        }

        /**
         * Returns the portion of the source that corresponds to this range
         * either as a string (for character types) or a vector (for non-character types).
         * 
         */
        auto get_source() const {
            if constexpr (is_char_v<value_type>) {
                return std::basic_string<value_type>(m_begin, m_end);
            }
            else {
                return std::vector<value_type>(m_begin, m_end);
            }
        }

    protected:
        /**
         * The default constructor.
         */
        source_range() {
        }

        /**
         * The constructor.
         * @param id id of the range.
         * @param begin begin iterator.
         * @param end end iterator.
         */
        source_range(const id_type& id, const iterator_type& begin, const iterator_type& end)
            : m_id(id)
            , m_begin(begin)
            , m_end(end)
        {
        }

    private:
        Id m_id;
        iterator_type m_begin;
        iterator_type m_end;
    };


    /**
     * Helper function for writing an id to a stream.
     * It can be overloaded for enumerations to print the exact enumeration name,
     * @param stream target stream.
     * @param id id to write.
     */
    template <class Stream>
    void to_string(Stream& stream, int id) {
        stream << id;
    }


    template <class T> struct has_begin_method {
        static constexpr bool value = false;
    };


    template <class T> struct is_ptr_to_object_with_begin_method {
        static constexpr bool value = false;
    };


    /**
     * Converts a source range to a string.
     * By default, it writes the first few values of the range.
     * If the value of the iterator has a 'begin' method, 
     * then prints the contents of the original source.
     * @param stream target stream.
     * @param begin begin of range.
     * @param end end of range.
     * @param max_length maximum length to write.
     */
    template <class Stream, class It>
    void to_string(Stream& stream, const It& begin, const It& end, size_t max_length = 10) {
        using value_type = std::decay_t<decltype(*begin)>;
        if constexpr (has_begin_method<value_type>::value) {
            to_string(stream, begin->begin(), std::prev(end)->end(), max_length);
        }
        else if constexpr (is_ptr_to_object_with_begin_method<value_type>::value) {
            to_string(stream, (*begin)->begin(), std::prev((*end))->end(), max_length);
        }
        else {
            stream << '"';
            size_t index = 0;
            It it = begin;
            for (; it != end && index < max_length; ++it, ++index) {
                stream << *it;
            }
            if (it != end) {
                stream << "...";
            }
            stream << '"';
        }
    }


} //namespace parserlib


#endif //PARSERLIB_SOURCE_RANGE_HPP
