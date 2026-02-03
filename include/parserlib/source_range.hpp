#ifndef PARSERLIB_SOURCE_RANGE_HPP
#define PARSERLIB_SOURCE_RANGE_HPP


#include <string>
#include <vector>
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


} //namespace parserlib


#endif //PARSERLIB_SOURCE_RANGE_HPP
