#ifndef PARSERLIB_PARSERINTERFACE_HPP
#define PARSERLIB_PARSERINTERFACE_HPP


namespace parserlib {


    /**
     * Interface for parsers allocated on the heap.
     * @param ParseContextType type of context to pass to the parse function.
     */
    template <class ParseContextType> class ParserInterface {
    public:
        /**
         * Virtual destructor due to polymorphism.
         */
        virtual ~ParserInterface() {
        }

        /**
         * Parsing interface. 
         * @param pc parse context.
         * @return parsing success.
         */
        virtual bool operator ()(ParseContextType& pc) const = 0;

        /**
         * Parses a terminal under left recursion.
         * @param pc parse context.
         * @return true if a terminal was parsed, false otherwise.
         */
        virtual bool parseLeftRecursionTerminal(ParseContextType& pc) const = 0;

        /**
         * Parses a left recursion continuation.
         * @param pc parse context.
         * @return true on success, false otherwise.
         */
        virtual bool parseLeftRecursionContinuation(ParseContextType& pc) const = 0;
    };


} //namespace parserlib


#endif //PARSERLIB_PARSERINTERFACE_HPP
