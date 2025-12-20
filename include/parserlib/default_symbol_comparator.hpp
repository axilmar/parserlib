#ifndef PARSERLIB_DEFAULT_SYMBOL_COMPARATOR_HPP
#define PARSERLIB_DEFAULT_SYMBOL_COMPARATOR_HPP


namespace parserlib {


    /**
     * The default symbol comparator.
     */
    class default_symbol_comparator {
    public:
        /**
         * Compares the left with the right operand.
         * Both operands must be convertible to `int`.
         * @param left the left operand.
         * @param right the right operand.
         * @return the difference of the two operands.
         */
        template <class L, class R>
        static int compare(const L& left, R& right) noexcept {
            return static_cast<int>(left) - static_cast<int>(right);
        }
    };


} //namespace parserlib


#endif //PARSERLIB_DEFAULT_SYMBOL_COMPARATOR_HPP
