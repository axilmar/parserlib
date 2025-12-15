#ifndef PARSERLIB_DEFAULT_SYMBOL_COMPARATOR_HPP
#define PARSERLIB_DEFAULT_SYMBOL_COMPARATOR_HPP


namespace parserlib {


    /**
     * The default symbol comparator.
     * It compares values 'as-is', i.e. without a conversion.
     */
    class default_symbol_comparator {
    public:
        /**
         * Returns the difference of the two objects, after casting them to 'int'.
         * @param a the first symbol.
         * @param b the second symbol.
         * @return the difference 'a - b' as an integer.
         */
        template <class A, class B>
        int operator ()(const A& a, const B& b) const {
            return static_cast<int>(a) - static_cast<int>(b);
        }
    };


} //namespace parserlib


#endif //PARSERLIB_DEFAULT_SYMBOL_COMPARATOR_HPP
