#ifndef PARSERLIB_CASE_INSENSITIVE_COMPARATOR_HPP
#define PARSERLIB_CASE_INSENSITIVE_COMPARATOR_HPP


#include <cctype>


namespace parserlib {


    /**
     * A token comparator which uses std::tolower to compare two tokens.
     */
    class case_insensitive_comparator {
    public:
        /**
         * Executes `std::tolower(l) - std::tolower(r)`.
         * @param l left operand.
         * @param r right operand.
         * @return the result of their subtraction, after they are converted to lowercase.
         */
        template <class L, class R>
        auto operator ()(const L& l, const R& r) const noexcept {
            return std::tolower(l) - std::tolower(r);
        }
    };


} //namespace parserlib


#endif //PARSERLIB_CASE_INSENSITIVE_COMPARATOR_HPP
