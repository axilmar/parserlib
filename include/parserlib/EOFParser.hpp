#ifndef PARSERLIB_EOFPARSER_HPP
#define PARSERLIB_EOFPARSER_HPP


#include "ParserNode.hpp"


namespace parserlib {


    /**
     * A parser that checks if there is no more input to parse. 
     */
    class EOFParser : public ParserNode<EOFParser> {
    public:
        /**
         * The parse function.
         * @param pc parse context.
         * @return true if there is no more input to parse, false otherwise.
         */
        template <class ParseContextType> bool operator ()(ParseContextType& pc) const {
            return pc.sourcePosition() == pc.sourceEndPosition();
        }
    };


    /**
     * Shortcut for creating an EOFParser.
     * @return an EOFParser instance.
     */
    inline EOFParser eof() {
        return {};
    }


} //namespace parserlib


#endif //PARSERLIB_EOFPARSER_HPP