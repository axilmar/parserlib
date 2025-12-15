#ifndef PARSERLIB_SOURCE_RANGE_HPP
#define PARSERLIB_SOURCE_RANGE_HPP


#include <cassert>
#include "parse_position.hpp"


namespace parserlib {


    /**
     * A class that represents a source range.
     * @param Id id type of the range.
     * @param Iterator type of iterator for the parse position.
     * @param SourcePosition type of source position for the parse position.
     */
    template <class Id, class Iterator, class SourcePosition> 
    class source_range {
    public:
        /** Type of id. */
        using id_type = Id;

        /** Type of parse position. */
        using parse_position_type = parse_position<Iterator, SourcePosition>;

        /**
         * The default constructor.
         */
        source_range() {
        }

        /**
         * Constructor from arguments.
         * @param id id of the range.
         * @param begin_parse_position the position where the range starts from.
         * @param end_parse_position the position where the range ends at.
         */
        source_range(const id_type& id, const parse_position_type& begin_parse_position, const parse_position_type& end_parse_position)
            : m_id(id)
            , m_begin_parse_position(begin_parse_position)
            , m_end_parse_position(end_parse_position)
        {
            assert(m_begin_parse_position < m_end_parse_position);
        }

        /**
         * Returns the id of the range.
         * @return the id of the range.
         */
        const id_type& get_id() const {
            return m_id;
        }

        /**
         * Returns the parse position from where the range starts from in the source.
         * @return the parse position from where the range starts from in the source.
         */
        const parse_position_type& get_begin_parse_position() const {
            return m_begin_parse_position;
        }

        /**
         * Returns the parse position from where the range ends at in the source.
         * @return the parse position from where the range ends at in the source.
         */
        const parse_position_type& get_end_parse_position() const {
            return m_end_parse_position;
        }

        /**
         * Returns a container that corresponds to the source.
         * @return a container that corresponds to the source.
         */
        template <class Container>
        Container get_source() const {
            return Container(m_begin_parse_position.get_iterator(), m_end_parse_position.get_iterator());
        }

    private:
        id_type m_id;
        parse_position_type m_begin_parse_position;
        parse_position_type m_end_parse_position;
    };


} //namespace parserlib


#endif //PARSERLIB_SOURCE_RANGE_HPP
