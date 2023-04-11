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
    };


} //namespace parserlib


#endif //PARSERLIB_PARSERINTERFACE_HPP
