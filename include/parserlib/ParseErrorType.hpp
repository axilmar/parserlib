#ifndef PARSERLIB_PARSEERRORTYPE_HPP
#define PARSERLIB_PARSEERRORTYPE_HPP


namespace parserlib {


    /**
     * The default parse error type.
     */
    enum class ParseErrorType {
        /**
         * The type of error provided by default by the library.
         */
        SyntaxError,

        /**
         * The first available value in order to extend the parse error types.
         */
        User
    };


} //namespace parserlib


#endif //PARSERLIB_PARSEERRORTYPE_HPP
