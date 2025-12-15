#ifndef PARSERLIB_MATCH_HPP
#define PARSERLIB_MATCH_HPP


#include <vector>
#include <utility>
#include "source_range.hpp"


namespace parserlib {


    /**
     * A class that represents a source range that is a match.
     * @param Id id type of the match.
     * @param Iterator type of iterator for the parse position.
     * @param SourcePosition type of source position for the parse position.
     */
    template <class Id, class Iterator, class SourcePosition> 
    class match : public source_range<Id, Iterator, SourcePosition> {
    public:
        /** The source range type. */
        using source_range_type = source_range<Id, Iterator, SourcePosition>;

        using typename source_range_type::id_type;
        using typename source_range_type::parse_position_type;

        /** The match type. */
        using match_type = match<Id, Iterator, SourcePosition>;

        /** The match container type. */
        using match_container_type = std::vector<match_type>;

        /**
         * The default constructor.
         */
        match() {
        }

        /**
         * Constructor from arguments.
         * @param id id of the range.
         * @param begin_parse_position the position where the range starts from.
         * @param end_parse_position the position where the range ends at.
         * @param children the children matches.
         */
        match(const id_type& id, const parse_position_type& begin_parse_position, const parse_position_type& end_parse_position, match_container_type&& children)
            : source_range_type(id, begin_parse_position, end_parse_position)
            , m_children(std::move(children))
        {
        }

        /**
         * Returns the children matches.
         * @return the children matches.
         */
        const match_container_type& get_children() const {
            return m_children;
        }

    private:
        match_container_type m_children;
    };


} //namespace parserlib


#endif //PARSERLIB_MATCH_HPP
