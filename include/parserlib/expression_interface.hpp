#ifndef PARSERLIB__EXPRESSION_INTERFACE__HPP
#define PARSERLIB__EXPRESSION_INTERFACE__HPP


#include "expression.hpp"
#include "parse_result.hpp"


namespace parserlib
{


    /**
        Base class for polymorphic interfaces to expressions.
        @param ParseContext type of parse context to use in parsing.
     */
    template <typename ParseContext>
    class expression_interface : public expression
    {
    public:
        ///destructor.
        virtual ~expression_interface()
        {
        }

        /**
            Parse interface.
            @param pc parse context.
            @return parse result.
         */
        virtual parse_result parse(ParseContext& pc) const = 0;
    };


} //namespace parserlib


#endif //PARSERLIB__EXPRESSION_INTERFACE__HPP
