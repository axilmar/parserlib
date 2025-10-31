#ifndef PARSERLIB_TUPLE_HPP
#define PARSERLIB_TUPLE_HPP


#include <tuple>


namespace parserlib {


    template <size_t StartIndex, size_t EndIndex, class... T> 
    auto get_tuple_mid(const std::tuple<T...>& tpl) {
        if constexpr (StartIndex < EndIndex) {
            return std::tuple_cat(std::make_tuple(std::get<StartIndex>(tpl)), get_tuple_mid<StartIndex + 1, EndIndex>(tpl));
        }
        else {
            return std::tuple<>();
        }
    }


    template <size_t EndIndex, class... T> 
    auto get_tuple_left(const std::tuple<T...>& tpl) {
        return get_tuple_mid<0, EndIndex>(tpl);
    }


    template <size_t StartIndex, class... T>
    auto get_tuple_right(const std::tuple<T...>& tpl) {
        return get_tuple_mid<StartIndex, std::tuple_size_v<std::tuple<T...>>>(tpl);
    }


} //namespace parserlib


#endif //PARSERLIB_TUPLE_HPP
