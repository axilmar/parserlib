#ifndef PARSERLIB_PARSE_DEFINITIONS_HPP
#define PARSERLIB_PARSE_DEFINITIONS_HPP


#include <string>
#include "case_sensitive_comparator.hpp"
#include "span.hpp"


namespace parserlib {


    /**
     * Parse definitions.
     * @param Input the source type.
     * @param OutputToken the output token type.
     * @param ErrorId the error id type.
     * @param TokenComparator the token comparator.
     */
    template <class Input = std::string, class OutputToken = int, class ErrorId = int, class TokenComparator = case_sensitive_comparator>
    class parse_definitions {
    public:
        /** Input type. */
        using input_type = Input;

        /** Input token type. */
        using input_token_type = typename input_type::value_type;

        /** Input iterator type. */
        using input_iterator_type = typename input_type::const_iterator;

        /** Input span type. */
        using input_span_type = span<input_iterator_type>;

        /** Input token comparator type. */
        using input_token_comparator_type = TokenComparator;

        /** Output token type. */
        using output_token_type = OutputToken;

        /** Error id type. */
        using error_id_type = ErrorId;
    };


} //namespace parserlib


#endif //PARSERLIB_PARSE_DEFINITIONS_HPP
