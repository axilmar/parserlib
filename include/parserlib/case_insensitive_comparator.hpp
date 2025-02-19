#ifndef PARSERLIB_CASE_INSENSITIVE_COMPARATOR_HPP
#define PARSERLIB_CASE_INSENSITIVE_COMPARATOR_HPP


#include <cctype>


namespace parserlib {


    class case_insensitive_comparator {
    public:
        template <class L, class R>
        auto operator ()(const L& l, const R& r) const noexcept {
            return std::tolower(l) - std::tolower(r);
        }
    };


} //namespace parserlib


#endif //PARSERLIB_CASE_INSENSITIVE_COMPARATOR_HPP
