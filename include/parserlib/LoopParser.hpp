#ifndef PARSERLIB_LOOPPARSER_HPP
#define PARSERLIB_LOOPPARSER_HPP


#include "ParserNode.hpp"


namespace parserlib {


    /**
     * A parser that invokes another parser in a loop.
     * @param ParserNodeType the parser to invoke in a loop.
     */
    template <class ParserNodeType> class LoopParser : public ParserNode<LoopParser<ParserNodeType>> {
    public:
        /**
         * The default constructor.
         * @param child child parser to invoke in a loop.
         */
        LoopParser(const ParserNodeType& child) : m_child(child) {
        }

        /**
         * Returns the parser to invoke in a loop.
         * @return the parser to invoke in a loop.
         */
        const ParserNodeType& child() const {
            return m_child;
        }

        /**
         * Invokes the child parser repeatedly, until no more parsing can succeed.
         * @param pc parse context.
         * @return always true.
         */
        template <class ParseContextType> bool operator ()(ParseContextType& pc) const {
            while (true) {
                if (!m_child(pc)) {
                    break;
                }
            }
            return true;
        }

    private:
        const ParserNodeType m_child;
    };


    /**
     * Converts a parser node into a loop.
     * @param node parser node to convert to a loop.
     * @return a loop parser node.
     */
    template <class ParserNodeType> 
    LoopParser<ParserNodeType> 
    operator *(const ParserNode<ParserNodeType>& node) {
        return LoopParser<ParserNodeType>(static_cast<const ParserNodeType&>(node));
    }


    /**
     * Avoids converting a loop parser node into another loop.
     * @param loop loop parser node.
     * @return the given loop parser node.
     */
    template <class ParserNodeType>
    const LoopParser<ParserNodeType>&
    operator *(const LoopParser<ParserNodeType>& loop) {
        return loop;
    }


} //namespace parserlib


#endif //PARSERLIB_LOOPPARSER_HPP
