#ifndef PARSERLIB_TUPLE_HPP
#define PARSERLIB_TUPLE_HPP


namespace parserlib {


    /*
     * Drop in replacement for std::tuple, which compiles very slowly.
     */


    /**
     * Generic tuple.
     */ 
    template <class T, class Base> struct tuple {
        static constexpr size_t size = 1 + Base::size;
        T value;
        Base base;
    };


    /**
     * Specialization for tuple without any members.
     */
    template <> struct tuple<void, void> {
        static constexpr size_t size = 0;
    };


    /**
     * Specialization for tuple with only one member.
     */
    template <class T> struct tuple<T, void> {
        static constexpr size_t size = 1;
        T value;
        tuple<void, void> base;
    };


    /**
     * Makes a tuple without any members.
     * @return a tuple without any members.
     */
    inline auto make_tuple() {
        return tuple<void, void>();
    }


    /**
     * Makes a tuple with one member.
     * @return a tuple with one member.
     */
    template <class T> auto make_tuple(const T& val) {
        return tuple<T, void>{val};
    }


    /**
     * Makes a tuple with multiple member values.
     * @param head first value.
     * @param tail rest of values.
     * @return a tuple with multiple member values.
     */
    template <class H, class... T> auto make_tuple(const H& head, const T&... tail) {
        return tuple<H, decltype(make_tuple(tail...))>{head, make_tuple(tail...)};
    }


    /**
     * Returns the size of a tuple.
     * @return the size of a tuple.
     */
    template <class Tpl>
    constexpr size_t tuple_size_v = Tpl::size;


    /**
     * Returns a tuple's member from index.
     * @param Index index of member.
     * @param tpl the tuple.
     */
    template <size_t Index, class T, class Base>
    const auto& get(const tuple<T, Base>& tpl) {
        if constexpr (Index == 0) {
            return tpl.value;
        }
        else {
            return get<Index - 1>(tpl.base);
        }
    }


    /**
     * Concatenation for single tuple.
     * @param tpl the tuple.
     * @reutrn the tuple.
     */ 
    template <class T, class Base>
    auto tuple_cat(const tuple<T, Base>& tpl) {
        return tpl;
    }


    /**
     * Concatenation of tuple with empty tuple.
     * @param head the tuple.
     * @param tail the empty tuple.
     * @return the tuple.
     */
    template <class T, class Base>
    auto tuple_cat(const tuple<T, Base>& head, const tuple<void, void>& tail) {
        return head;
    }


    /**
     * Concatenation of empty tuple with tuple.
     * @param head the empty tuple.
     * @param tail the tuple.
     * @return the tuple.
     */
    template <class T, class Base>
    auto tuple_cat(const tuple<void, void>& head, const tuple<T, Base>& tail) {
        return tail;
    }


    /**
     * Concatenation of two or more tuples.
     * @param head the head tuple.
     * @param tail the rest of the tuples.
     * @return a tuple with all the specified tuples concatenated.
     */
    template <class H, class... T>
    auto tuple_cat(const H& head, const T&... tail) {
        return tuple<decltype(head.value), decltype(parserlib::tuple_cat(head.base, parserlib::tuple_cat(tail...)))>{head.value, parserlib::tuple_cat(head.base, parserlib::tuple_cat(tail...))};
    }


} //namespace parserlib


#endif //PARSERLIB_TUPLE_HPP
