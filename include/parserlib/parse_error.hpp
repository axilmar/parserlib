#ifndef PARSERLIB_PARSE_ERROR_HPP
#define PARSERLIB_PARSE_ERROR_HPP


#include "source_range.hpp"


namespace parserlib {


    /**
     * A class that represents a source range that is a parse error.
     * @param Id id type of the error.
     * @param Iterator type of iterator for the parse position.
     * @param SourcePosition type of source position for the parse position.
     */
    template <class Id, class Iterator, class SourcePosition> 
    class parse_error : public source_range<Id, Iterator, SourcePosition> {
    public:
        /** The source range type. */
        using source_range_type = source_range<Id, Iterator, SourcePosition>;

        using typename source_range_type::id_type;
        using typename source_range_type::parse_position_type;

        /**
         * The default constructor.
         */
        parse_error() {
        }

        /**
         * Constructor from arguments.
         * @param id id of the range.
         * @param begin_parse_position the position where the range starts from.
         * @param end_parse_position the position where the range ends at.
         */
        parse_error(const id_type& id, const parse_position_type& begin_parse_position, const parse_position_type& end_parse_position)
            : source_range_type(id, begin_parse_position, end_parse_position)
        {
        }
    };


} //namespace parserlib


#endif //PARSERLIB_PARSE_ERROR_HPP
