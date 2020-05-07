#ifndef PARSERLIB_PARSELOOP_HPP
#define PARSERLIB_PARSELOOP_HPP


namespace parserlib
{


    /**
        Parse loop implementation.
        Parses the input until the given expression returns false
        or no terminal is consumed.
        @param expression expression to parse.
        @param parseContext parse context.
        @return the result of the last parse.
     */
    template <class ExpressionType, class ParseContextType>
    bool parseLoop(const ExpressionType& expression, ParseContextType& parseContext)
    {
        //must parse from valid position
        while (parseContext.isValidPosition())
        {
            const auto startPosition = parseContext.getCurrentPosition();
                
            //if the expression failed to parse, stop the loop with failure
            if (!expression.parse(parseContext))
            {
                return false;
            }

            //if the expression parsed to true but there was no terminal consumed,
            //also stop the loop, but with success
            if (startPosition == parseContext.getCurrentPosition())
            {
                return true;
            }
        }

        //failure because no input was available
        return false;
    }


} //namespace parserlib


#endif //PARSERLIB_PARSELOOP_HPP
