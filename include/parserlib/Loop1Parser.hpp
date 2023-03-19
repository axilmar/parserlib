#ifndef PARSERLIB_LOOP1PARSER_HPP
#define PARSERLIB_LOOP1PARSER_HPP


#include "ParserNode.hpp"


namespace parserlib {


    /**
     * A parser that invokes another parser in a loop; the loop must suceed at least once.
     * @param ParserNodeType the parser to invoke in a loop.
     */
    template <class ParserNodeType> class Loop1Parser : public ParserNode<Loop1Parser<ParserNodeType>> {
    public:
        /**
         * The default constructor.
         * @param child child parser to invoke in a loop.
         */
        Loop1Parser(const ParserNodeType& child) : m_child(child) {
        }

        /**
         * Returns the child parser to invoke in a loop.
         * @return the child parser to invoke in a loop.
         */
        const ParserNodeType& child() const {
            return m_child;
        }

        /**
         * Invokes the child parser once. If that succeeds,  
         * then it invokes the child parser repeatedly, until no more parsing can succeed.
         * @param pc parse context.
         * @return true if the 1st parsing succeeded, false otherwise.
         */
        template <class ParseContextType> bool operator ()(ParseContextType& pc) const {
            if (!m_child(pc)) {
                return false;
            }
            while (true) {
                if (!m_child(pc)) {
                    break;
                }
            }
            return true;
        }

    private:
        ParserNodeType m_child;
    };


    /**
     * Converts a parser node into a loop.
     * @param node parser node to convert to a loop.
     * @return a loop parser node.
     */
    template <class ParserNodeType> 
    Loop1Parser<ParserNodeType> 
    operator +(const ParserNode<ParserNodeType>& node) {
        return Loop1Parser<ParserNodeType>(static_cast<const ParserNodeType&>(node));
    }


    /**
     * Avoids converting a loop parser node into another loop.
     * @param loop loop parser node.
     * @return the given loop parser node.
     */
    template <class ParserNodeType>
    const Loop1Parser<ParserNodeType>&
    operator +(const Loop1Parser<ParserNodeType>& loop) {
        return loop;
    }


} //namespace parserlib


#endif //PARSERLIB_LOOP1PARSER_HPP
