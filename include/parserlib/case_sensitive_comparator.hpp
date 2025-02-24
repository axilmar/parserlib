#ifndef PARSERLIB_DEFAULT_TOKEN_COMPARATOR_HPP
#define PARSERLIB_DEFAULT_TOKEN_COMPARATOR_HPP


namespace parserlib {


    /**
     * A comparator which compares two tokens without any conversions.
     */
    class case_sensitive_comparator {
    public:
        /**
         * Executes `l - r`.
         * @param l left operand.
         * @param r right operand.
         * @return the result of their subtraction; no conversion is performed.
         */
        template <class L, class R>
        auto operator ()(const L& l, const R& r) const noexcept {
            return l - r;
        }
    };


} //namespace parserlib


#endif //PARSERLIB_DEFAULT_TOKEN_COMPARATOR_HPP
