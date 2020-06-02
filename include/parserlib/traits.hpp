#ifndef PARSERLIB__TRAITS__HPP
#define PARSERLIB__TRAITS__HPP


#include <type_traits>


namespace parserlib
{


    template<typename ...> using to_void = void;


    /**
        Checks if T is a container (i.e. if it contains begin() and end()).
        This is the 'false' version for non-containers.
        @param T type to check.
     */
    template <typename T, typename = void> 
    struct is_container 
        : public std::false_type
    {
    };


    /**
        Checks if T is a container (i.e. if it contains begin() and end()).
        This is the 'false' version for containers.
        @param T type to check.
     */
    template <typename T> 
    struct is_container<
        T, 
        to_void<decltype(std::declval<T>().begin()),decltype(std::declval<T>().end())>>
        : public std::true_type
    {
    };


    /**
        Helper for determining at compile time if T is a container or not.
        @param T type to check.
     */
    template <typename T>
    inline constexpr bool is_container_v = is_container<T>::value;


} //namespace parserlib


#endif //PARSERLIB__TRAITS__HPP
