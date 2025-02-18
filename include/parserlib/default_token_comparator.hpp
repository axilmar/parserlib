#ifndef PARSERLIB_DEFAULT_TOKEN_COMPARATOR_HPP
#define PARSERLIB_DEFAULT_TOKEN_COMPARATOR_HPP


#include <type_traits>


namespace parserlib {


    class default_token_comparator {
    public:
        template <class L, class R>
        auto operator ()(const L& l, const R& r) const noexcept {
            if constexpr ((std::is_integral_v<L>) && std::is_integral_v<R>) {
                return l - r;
            }
            else {
                return l < r ? -1 : l > r ? 1 : 0;
            }
        }
    };


} //namespace parserlib


#endif //PARSERLIB_DEFAULT_TOKEN_COMPARATOR_HPP
