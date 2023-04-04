#ifndef PARSERLIB_TREEMATCHEXCEPTION_HPP
#define PARSERLIB_TREEMATCHEXCEPTION_HPP


#include <stdexcept>


namespace parserlib {


    /**
     * Exception thrown when a tree match context requests an invalid number of children.
     * @param ParseContextType parse context type.
     */
    template <class ParseContextType> class TreeMatchException : public std::runtime_error {
    public:
        /**
         * The constructor.
         * @param pc parse context.
         */
        TreeMatchException(ParseContextType& pc) : std::runtime_error("Match tree mismatch."), m_parseContext(pc) {
        }

        /**
         * Returns the parse context.
         * @return the parse context.
         */
        const ParseContextType& parseContext() const {
            return m_parseContext;
        }

        /**
         * Returns the parse context.
         * @return the parse context.
         */
        ParseContextType& parseContext() {
            return m_parseContext;
        }

    private:
        ParseContextType& m_parseContext;
    };


} //namespace parserlib


#endif //PARSERLIB_TREEMATCHEXCEPTION_HPP
