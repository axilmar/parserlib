#ifndef PARSERLIB_CORE_PARSEERRORTYPE_HPP
#define PARSERLIB_CORE_PARSEERRORTYPE_HPP


namespace parserlib::core {


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


} //namespace parserlib::core


#endif //PARSERLIB_CORE_PARSEERRORTYPE_HPP
