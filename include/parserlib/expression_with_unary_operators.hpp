#ifndef PARSERLIB__EXPRESSION_WITH_UNARY_OPERATORS__HPP
#define PARSERLIB__EXPRESSION_WITH_UNARY_OPERATORS__HPP


#include "expression_type.hpp"


namespace parserlib
{


    /**
        Base class for expressions that have unary operators.
        @param Derived type of derived class.
     */
    template <typename Derived> 
    class expression_with_unary_operators : public expression
    {
    public:
    };


} //namespace parserlib


#endif //PARSERLIB__EXPRESSION_WITH_UNARY_OPERATORS__HPP
