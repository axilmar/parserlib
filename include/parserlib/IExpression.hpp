#ifndef PARSERLIB_IEXPRESSION_HPP
#define PARSERLIB_IEXPRESSION_HPP


namespace parserlib
{


    /**
        Interface to an expression.
        @param ParseContextType type of parse context.
     */
    template <typename ParseContextType> class IExpression
    {
    public:
        ///destructor.
        virtual ~IExpression()
        {
        }

        /**
            Parse interface.
            @param pc parse context.
            @return parse success/failure.
         */
        virtual bool parse(ParseContextType& pc) const = 0;
    };


} //namespace parserlib


#endif //PARSERLIB_IEXPRESSION_HPP
