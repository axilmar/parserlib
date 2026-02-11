#ifndef PARSERLIB_SYMBOL_COMPARATOR_HPP
#define PARSERLIB_SYMBOL_COMPARATOR_HPP


#include "symbol.hpp"


namespace parserlib {


    /**
     * The default comparator simply returns the difference between two symbol values.
     */ 
    class default_symbol_comparator {
    public:
        /**
         * Returns the result of `left - right`.
         * @return the result of `left - right`.
         */
        static symbol_value_type compare(symbol_value_type left, symbol_value_type right);
    };


    /**
     * It converts the symbols to lowercase, using std::tolower,
     * before subtracting them.
     */
    class case_insensitive_symbol_comparator {
    public:
        /**
         * Converts both symbols to lowercase using std::tolower,
         * then subtracts the values and returns the difference.
         * @return the result of `std::tolower(left) - std::tolower(right)`.
         */ 
        static symbol_value_type compare(symbol_value_type left, symbol_value_type right);
    };


} //namespace parserlib


#endif //PARSERLIB_SYMBOL_COMPARATOR_HPP
