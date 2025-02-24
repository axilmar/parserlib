#ifndef PARSERLIB_LESS_HPP
#define PARSERLIB_LESS_HPP


namespace parserlib {


    /**
     * A less comparator function that uses the given comparator for testing.
     * Used in calling `std::upper_bound` for quick searching within sorted vectors.
     * @param Comparator a comparator that must return a numeric value representing
     *  the difference between the given operands. The less comparator tests if this value 
     *  is less than 0.
     */
    template <class Comparator>
    class less {
    public:
        /**
         * Executes `(l - r) < 0`.
         * @param l left operand.
         * @param r right operand.
         * @return true if the result of their subtraction is less than 0, false otherwise.
         */
        template <class L, class R>
        bool operator ()(const L& l, const R& r) const noexcept {
            return Comparator()(l, r) < 0;
        }
    };


} //namespace parserlib


#endif //PARSERLIB_LESS_HPP
