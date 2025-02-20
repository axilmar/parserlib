#ifndef PARSERLIB_IS_SEQUENCE_CONTAINER_HPP
#define PARSERLIB_IS_SEQUENCE_CONTAINER_HPP


#include <type_traits>


namespace parserlib {


    template <class T>
    struct is_container {
    private:
        using result_false = char;
        struct result_true { char c[2]; };
        template <class U> static result_false test_begin(...) { return {}; }
        template <class U> static result_true test_begin(decltype(&U::cbegin)) { return {}; }
        template <class U> static result_false test_end(...) { return {}; }
        template <class U> static result_true test_end(decltype(&U::cend)) { return {}; }

    public:
        static constexpr bool value = sizeof(test_begin<T>(0)) == sizeof(result_true) && sizeof(test_end<T>(0)) == sizeof(result_true);
    };


    template <class T>
    constexpr bool is_container_v = is_container<T>::value;


} //namespace parserlib


#endif //PARSERLIB_IS_SEQUENCE_CONTAINER_HPP
