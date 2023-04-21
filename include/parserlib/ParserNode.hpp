#ifndef PARSERLIB_PARSERNODE_HPP
#define PARSERLIB_PARSERNODE_HPP


#include "LeftRecursionContext.hpp"


namespace parserlib {


    /**
     * Generic, type-unware parser node base class. 
     */
    class ParserNodeBase {
    public:
    };


    /**
     * Base class for parser nodes.
     * @param ParserNodeType type of parser node derived from this.
     */
    template <class ParserNodeType> class ParserNode : public ParserNodeBase {
    public:
    };


} //namespace parserlib


#endif //PARSERLIB_PARSERNODE_HPP
