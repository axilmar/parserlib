#ifndef PARSERLIB_TUPLE_HPP
#define PARSERLIB_TUPLE_HPP


#include <tuple>


namespace parserlib {


    template <class Impl> class parse_node;


    /**
     * Iterates the given tuple and calls the given function for each member.
     * @param Index index to start the iteration from.
     * @param tpl the tuple.
     * @param fn the function.
     */
    template <size_t Index, class Tpl, class F>
    void tuple_for_each(const Tpl& tpl, const F& fn) {
        if constexpr (Index < std::tuple_size_v<Tpl>) {
            fn(std::get<Index>(tpl));
            tuple_for_each<Index + 1>(tpl, fn);
        }        
    }


    /**
     * Iterates the given tuple and calls the given function for each member.
     * If the function returns the same value as the result, then
     * the function is invoked for the next tuple members;
     * otherwise, the opposite of the given result is returned.
     * If all members of the tuple are processed,
     * then the given result is returned.
     * @param Index index to start the iteration from.
     * @param Result the result to return.
     * @param tpl the tuple.
     * @param fn the function.
     */
    template <size_t Index, bool Result, class Tpl, class F>
    bool tuple_for_each_cond(const Tpl& tpl, const F& fn) {
        if constexpr (Index < std::tuple_size_v<Tpl>) {
            if (fn(std::get<Index>(tpl)) == Result) {
                return tuple_for_each_cond<Index + 1, Result>(tpl, fn);
            }
            else {
                return !Result;
            }
        }       
        else {
            return Result;
        }
    }


    template <class T>
    auto make_parse_node_tuple(const T& value) {
        return std::make_tuple(make_parse_node(value));
    }


    template <class Impl>
    const std::tuple<Impl> make_parse_node_tuple(const parse_node<Impl>& parse_node) {
        return std::make_tuple(*parse_node.get_impl());
    }


    template <class L, class R>
    auto make_parse_node_tuple(const L& left, const R& right) {
        return std::tuple_cat(make_parse_node_tuple(left), make_parse_node_tuple(right));
    }


} //namespace parserlib


#endif //PARSERLIB_TUPLE_HPP
