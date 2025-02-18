#ifndef PARSERLIB_PARSE_DEFINITIONS_HPP
#define PARSERLIB_PARSE_DEFINITIONS_HPP


#include <string>
#include "default_token_comparator.hpp"
#include "span.hpp"


namespace parserlib {


    template <class Source = std::string, class OutputToken = int, class ErrorId = int, class TokenComparator = default_token_comparator>
    class parse_definitions {
    public:
        using source_type = Source;

        using input_token_type = typename Source::value_type;

        using input_iterator_type = typename Source::const_iterator;

        using input_span_type = span<input_iterator_type>;

        using input_token_comparator_type = TokenComparator;

        using output_token_type = OutputToken;

        using error_id_type = ErrorId;
    };


} //namespace parserlib


#endif //PARSERLIB_PARSE_DEFINITIONS_HPP
