#ifndef PARSERLIB_ISTREAM_PARSE_CONTEXT_HPP
#define PARSERLIB_ISTREAM_PARSE_CONTEXT_HPP


#include <sstream>
#include <iterator>
#include "source_range_parse_context.hpp"


namespace parserlib {


    /**
     * A parse context class used for parsing source in an input stream.
     * @param Source stream type.
     * @param MatchId match id type.
     * @param ErrorId error id type.
     * @param SourcePosition source position type.
     * @param SymbolComparator symbol comparator type.
     */
    template <
        class Source = std::stringstream, 
        class MatchId = int, 
        class ErrorId = int, 
        class SourcePosition = default_source_position, 
        class SymbolComparator = default_symbol_comparator
    >
    class istream_parse_context 
        : public source_range_parse_context<std::istreambuf_iterator<typename Source::char_type>, MatchId, ErrorId, SourcePosition, SymbolComparator>
    {
    public:
        /** Iterator type. */
        using iterator_type = std::istreambuf_iterator<typename Source::char_type>;

        /** Base class type. */ 
        using range_parse_context_type = source_range_parse_context<iterator_type, MatchId, ErrorId, SourcePosition, SymbolComparator>;

        /**
         * Constructor from iterator range.
         * @param begin start of range to parse.
         * @param end end of range to parse.
         */
        istream_parse_context(const iterator_type& begin, const iterator_type& end = iterator_type())
            : range_parse_context_type(begin, end)
        {
        }

        /**
         * Constructor from container.
         * @param source the container to parse.
         */
        istream_parse_context(Source& source)
            : istream_parse_context(iterator_type(source))
        {
        }
    };


} //namespace parserlib


#endif //PARSERLIB_ISTREAM_PARSE_CONTEXT_HPP
