#ifndef PARSERLIB_PARSERWRAPPER_HPP
#define PARSERLIB_PARSERWRAPPER_HPP


#include "ParserInterface.hpp"


namespace parserlib {


    /**
     * Wraps a parser with a parser interface.
     * @param ParseContextType type of context to pass to the parse function.
     * @param ParserNodeType type of parser to wrap this class around.
     */
    template <class ParseContextType, class ParserNodeType> class ParserWrapper 
        : public ParserInterface<ParseContextType> {
    public:
        /**
         * Constructor.
         * @param parser parser to invoke.
         */
        ParserWrapper(const ParserNodeType& parser) : m_parser(parser) {
        }

        /**
         * Returns the wrapped parser.
         * @return the wrapped parser.
         */
        const ParserNodeType& parser() const {
            return m_parser;
        }

        /**
         * Uses the wrapped parser to parse.
         * @param pc parse context.
         * @return whatever the wrapped parser returns.
         */
        bool operator ()(ParseContextType& pc) const override {
            return m_parser(pc);
        }

        /**
         * Parses a terminal under left recursion.
         * @param pc parse context.
         * @return true if a terminal was parsed, false otherwise.
         */
        bool parseLeftRecursionTerminal(ParseContextType& pc) const override {
            return m_parser.parseLeftRecursionTerminal(pc);
        }

        /**
         * Parses a left recursion continuation.
         * @param pc parse context.
         * @return true on success, false otherwise.
         */
        bool parseLeftRecursionContinuation(ParseContextType& pc) const override {
            return m_parser.parseLeftRecursionContinuation(pc);
        }

    private:
        const ParserNodeType m_parser;
    };


} //namespace parserlib


#endif //PARSERLIB_PARSERWRAPPER_HPP
