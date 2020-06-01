#ifndef PARSERLIB__EXPRESSION_TYPE__HPP
#define PARSERLIB__EXPRESSION_TYPE__HPP


#include "expression.hpp"


namespace parserlib
{


    /**
        Type trait that allows the customization of expression types in grammar expressions.
        @param T type that needs an expression type.
     */
    template <typename T> 
    class expression_type
    {
    public:
        ///the default expression type for a parser is itself.
        typedef T type;
    };


    /**
        Specialization for const types.
        @param T type of parsing class that needs customization.
     */
    template <typename T> 
    class expression_type<const T>
    {
    public:
        ///removes const from the type.
        typename expression_type<T>::type type;
    };


    /**
        Specialization for reference types.
        @param T type of parsing class that needs customization.
     */
    template <typename T> 
    class expression_type<T&>
    {
    public:
        ///removes the reference from the type.
        typename expression_type<T>::type type;
    };


    /**
        Specialization for rvalue types.
        @param T type of parsing class that needs customization.
     */
    template <typename T> 
    class expression_type<T&&>
    {
    public:
        ///removes the rvalue from the type.
        typename expression_type<T>::type type;
    };


    /**
        Helper type for accessing the type member of expression_type of a type.
        @param T type to get the expression type of.
     */
    template <typename T> 
    using expression_type_t = typename expression_type<T>::type;


    /**
        Helper variable for checking if a type has an expression type.
        @param T type to check.
     */
    template <typename T>
    inline constexpr bool has_expression_type_v = is_expression_v<expression_type_t<T>>;


    /**
        Helper variable for checking if one or more types have expression types.
        @param ...T types to check.
     */
    template <typename ...T>
    inline constexpr bool have_expression_types_v = (... && has_expression_type_v<T>);


} //namespace parserlib


#endif //PARSERLIB__EXPRESSION_TYPE__HPP
