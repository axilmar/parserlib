#ifndef PARSERLIB_MATCH_HPP
#define PARSERLIB_MATCH_HPP


#include <vector>
#include <utility>
#include "source_range.hpp"


namespace parserlib {


    template <class Id = int, class Iterator = typename std::string::const_iterator>
    class match : public source_range<Id, Iterator> {
    public:
        /** id type. */
        using id_type = Id;

        /** iterator type. */
        using iterator_type = Iterator;

        /** match type. */
        using match_type = match<id_type, iterator_type>;

        /** match container type. */
        using match_container_type = std::vector<match_type>;

        /**
         * The default constructor.
         */
        match() noexcept {
        }

        /**
         * The constructor.
         * @param id id of the range.
         * @param begin begin iterator.
         * @param end end iterator.
         * @param children children matches.
         */
        match(const id_type& id, const iterator_type& begin, const iterator_type& end, match_container_type&& children)
            : source_range<Id, Iterator>(id, begin, end)
            , m_children(std::move(children))
        {
        }

        /**
         * Returns the id of the match as an `int`.
         * @return the id of the match as an `int`.
         */
        operator int () const noexcept {
            return static_cast<int>(this->get_id());
        }

        /**
         * Returns the children matches.
         * @return the children matches.
         */
        const match_container_type& get_children() const noexcept {
            return m_children;
        }

    private:
        match_container_type m_children;
    };


} //namespace parserlib


#endif //PARSERLIB_MATCH_HPP
