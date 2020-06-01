#ifndef PARSERLIB__EXPRESSION_INTERFACE__HPP
#define PARSERLIB__EXPRESSION_INTERFACE__HPP


#include "expression.hpp"


namespace parserlib
{


    /**
        Base class for polymorphic interfaces to expressions.
     */
    class expression_interface : public expression
    {
    public:
        ///destructor.
        virtual ~expression_interface()
        {
        }
    };


} //namespace parserlib


#endif //PARSERLIB__EXPRESSION_INTERFACE__HPP
