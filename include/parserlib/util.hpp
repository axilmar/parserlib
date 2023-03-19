#ifndef PARSERLIB_UTIL_HPP
#define PARSERLIB_UTIL_HPP


#include <tuple>


namespace parserlib {


    /**
     * Tuple iteration with conditional function.
     * @param I start index.
     * @param tpl tuple.
     * @param func func to apply.
     * @return what the function returns or not R if no function returns R.
     */
    template <size_t I, bool R, class F, class ...T> 
    auto tuple_for_each_cond(const std::tuple<T...>& tpl, const F& func) {
        if constexpr (I < std::tuple_size_v<std::tuple<T...>>) {
            if (func(std::get<I>(tpl)) == R) {
                return R;
            }
            return tuple_for_each_cond<I + 1, R>(tpl, func);
        }
        else {
            return !R;
        }
    }


} //namespace parserlib


#endif //PARSERLIB_UTIL_HPP
