#ifndef PARSERLIB_SYMBOL_COMPARATOR_HPP
#define PARSERLIB_SYMBOL_COMPARATOR_HPP


#include <cctype>


namespace parserlib {


    /**
     * The default comparator simply returns the difference between two symbol values.
     */ 
    class default_symbol_comparator {
    public:
        /**
         * Returns the result of `left - right`.
         * Both operands must be statically converted to `int`.
         * @param left the left operand.
         * @param right the right operand.
         * @return the result of `left - right`.
         */
        template <class L, class R>
        static int compare(const L& left, const R& right) {
            return static_cast<int>(left) - static_cast<int>(right);
        }
    };


    /**
     * It converts the symbols to lowercase, using std::tolower,
     * before subtracting them.
     */
    class case_insensitive_symbol_comparator {
    public:
        /**
         * Returns the result of `left - right`, after both converted to lowercase using `std::tolower`.
         * Both operands must be statically converted to `int`.
         * @param left the left operand.
         * @param right the right operand.
         * @return the result of `left - right`, after both converted to lowercase using `std::tolower`.
         */ 
        template <class L, class R>
        static int compare(const L& left, const R& right) {
            return std::tolower(static_cast<int>(left)) - std::tolower(static_cast<int>(right));
        }
    };


} //namespace parserlib


#endif //PARSERLIB_SYMBOL_COMPARATOR_HPP
