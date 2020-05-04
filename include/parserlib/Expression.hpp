#ifndef PARSERLIB_EXPRESSION_HPP
#define PARSERLIB_EXPRESSION_HPP


namespace parserlib
{


    /**
        Base class for expressions.
     */
    class Expression
    {
    public:
    };


    /**
        Expression type trait.
        Allows customization of expression types based on other types.
        @param T type to provide an expression type for.
     */
    template <class T> class ExpressionType
    {
    public:
        ///expression type.
        typedef T Type;
    };


    /**
        Shortcut for the expression type trait.
        @param T type to provide an expression type for.
     */
    template <class T>
    using ExpressionTypeT = typename ExpressionType<T>::Type;


} //namespace parserlib


#endif //PARSERLIB_EXPRESSION_HPP
