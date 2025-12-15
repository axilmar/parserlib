#ifndef CONTAINER_PARSE_CONTEXT_HPP
#define CONTAINER_PARSE_CONTEXT_HPP


#include "source_range_parse_context.hpp"


namespace parserlib {


    /**
     * A parse context class used for parsing source in a container.
     * @param Source container type.
     * @param MatchId match id type.
     * @param ErrorId error id type.
     * @param SourcePosition source position type.
     * @param SymbolComparator symbol comparator type.
     */
    template <
        class Source = std::string, 
        class MatchId = int, 
        class ErrorId = int, 
        class SourcePosition = default_source_position, 
        class SymbolComparator = default_symbol_comparator
    >
    class container_parse_context 
        : public source_range_parse_context<typename Source::const_iterator, MatchId, ErrorId, SourcePosition, SymbolComparator>
    {
    public:
        /** Iterator type. */
        using iterator_type = typename Source::const_iterator;

        /** Base class type. */ 
        using range_parse_context_type = source_range_parse_context<iterator_type, MatchId, ErrorId, SourcePosition, SymbolComparator>;

        /**
         * Constructor from iterator range.
         * @param begin start of range to parse.
         * @param end end of range to parse.
         */
        container_parse_context(const iterator_type& begin, const iterator_type& end)
            : range_parse_context_type(begin, end)
        {
        }

        /**
         * Constructor from container.
         * @param source the container to parse.
         */
        container_parse_context(Source& source)
            : container_parse_context(source.begin(), source.end())
        {
        }
    };


} //namespace parserlib


#endif //CONTAINER_PARSE_CONTEXT_HPP
