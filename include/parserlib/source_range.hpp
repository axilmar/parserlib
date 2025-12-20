#ifndef PARSERLIB_SOURCE_RANGE_HPP
#define PARSERLIB_SOURCE_RANGE_HPP


#include <string>


namespace parserlib {


    /**
     * Base class for source ranges.
     * @param Id id of range.
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

    protected:
        /**
         * The default constructor.
         */
        source_range() noexcept {
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
