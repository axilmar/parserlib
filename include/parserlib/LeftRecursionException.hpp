#ifndef PARSERLIB_LEFTRECURSIONEXCEPTION_HPP
#define PARSERLIB_LEFTRECURSIONEXCEPTION_HPP


namespace parserlib {


    template <class ParseContextType> class Rule;


    /**
     * Exception thrown when parsing a rule results in infinite recursion.
     * @param ParseContextType type of context to pass to the parse function.
     */
    template <class ParseContextType> class LeftRecursionException : public std::runtime_error {
    public:
        /**
         * Constructor.
         * @param rule rule that caused the exception.
         */
        LeftRecursionException(Rule<ParseContextType>& rule) : std::runtime_error("Left Recursion"), m_rule(rule) {
        }

        /**
         * Returns the rule that caused the exception.
         * @return the rule that caused the exception.
         */
        Rule<ParseContextType>& rule() const {
            return m_rule;
        }

    private:
        Rule<ParseContextType>& m_rule;
    };


} //namespace parserlib


#endif //PARSERLIB_LEFTRECURSIONEXCEPTION_HPP
