#ifndef PARSERLIB_CASE_SENSITIVE_SYMBOL_COMPARATOR_HPP
#define PARSERLIB_CASE_SENSITIVE_SYMBOL_COMPARATOR_HPP


#include <cctype>


namespace parserlib {


    /**
     * A case sensitive symbol comparator.
     * It compares values by converting them to lowercase, using the function 'std::tolower'.
     */
    class case_sensitive_symbol_comparator {
    public:
        /**
         * Returns the difference of the two objects, after casting them to 'int' and lowering their case using 'std::tolower'.
         * @param a the first symbol.
         * @param b the second symbol.
         * @return the difference 'a - b' as an integer.
         */
        template <class A, class B>
        static int compare(const A& a, const B& b) noexcept {
            return std::tolower(static_cast<int>(a)) - std::tolower(static_cast<int>(b));
        }
    };


} //namespace parserlib


#endif //PARSERLIB_CASE_SENSITIVE_SYMBOL_COMPARATOR_HPP
