#ifndef PARSERLIB__EXPRESSION__HPP
#define PARSERLIB__EXPRESSION__HPP


#include <type_traits>


namespace parserlib
{


    /**
        Base class for all grammar expressions.
     */
    class expression
    {
    public:
    };


    /**
        Trait that is used to identify if a type inherits from class expression.
        @param T type to test if it inherits from class expression.
     */
    template <typename T>
    inline constexpr bool is_expression_v = std::is_base_of_v<expression, T>;


} //namespace parserlib


#endif //PARSERLIB__EXPRESSION__HPP
