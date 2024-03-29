#ifndef PARSERLIB_EBNF_HPP
#define PARSERLIB_EBNF_HPP


#include "parserlib/Match.hpp"
#include "parserlib/LineCountingSourcePosition.hpp"


namespace parserlib::ebnf {


    /**
     * EBNF match ids.
     */
    enum class EBNF {
        /**
         * Rule.
         */
        RULE,

        /**
         * Alternation.
         */
        ALTERNATION,

        /**
         * Concatenation.
         */
        CONCATENATION,

        /**
         * Optional.
         */
        TERM_OPTIONAL,

        /**
         * Repeated.
         */
        TERM_REPEATED,

        /**
         * Grouped.
         */
        TERM_GROUPED,

        /**
         * Optional postfix.
         */
        TERM_OPTIONAL_POSTFIX,

        /**
         * Repeated 0 or more times, postfix.
         */
        TERM_REPEATED_0_OR_MORE_POSTFIX,

        /**
         * Repeated 1 or more times, postfix.
         */
        TERM_REPEATED_1_OR_MORE_POSTFIX,

        /**
         * Exception.
         */
        EXCEPTION,

        /**
         * Terminal.
         */
        TERMINAL,

        /**
         * Identifier.
         */
        IDENTIFIER
    };


    /**
     * Match type for EBNF parser.
     */
    using Match = parserlib::Match<std::string, EBNF, LineCountingSourcePosition<std::string>>;


} //namespace parserlib::ebnf


#endif //PARSERLIB_EBNF_HPP
