#ifndef PARSERLIB_NOTPARSER_HPP
#define PARSERLIB_NOTPARSER_HPP


#include "ParserNode.hpp"


namespace parserlib {


    /**
     * A parser that invokes another parser to validate the inverse of a grammar; does not consume input.
     * @param ParserNodeType the parser to invoke.
     */
    template <class ParserNodeType> class NotParser : public ParserNode<NotParser<ParserNodeType>> {
    public:
        /**
         * The default constructor.
         * @param child child parser to invoke.
         */
        NotParser(const ParserNodeType& child) : m_child(child) {
        }

        /**
         * Returns the parser to invoke.
         * @return the parser to invoke.
         */
        const ParserNodeType& child() const {
            return m_child;
        }

        /**
         * Invokes the child parser, then returns the opposite of the result.
         * The parser state is restored after the invocation of the child parser.
         * @param pc parse context.
         * @return true if parsing succeeds, false otherwise.
         */
        template <class ParseContextType> bool operator ()(ParseContextType& pc) const {
            const auto state = pc.state();
            const bool result = !m_child(pc);
            pc.setState(state);
            return result;
        }

    private:
        const ParserNodeType m_child;
    };


    /**
     * Converts a parser node into a not parser.
     * @param node parser node to convert to not.
     * @return a not parser node.
     */
    template <class ParserNodeType> 
    NotParser<ParserNodeType> 
    operator !(const ParserNode<ParserNodeType>& node) {
        return NotParser<ParserNodeType>(static_cast<const ParserNodeType&>(node));
    }


    /**
     * Converts a not parser node to its inverse, which is the not node's child node.
     * @param node not parser node.
     * @return the child of the given parser node.
     */
    template <class ParserNodeType>
    ParserNodeType
    operator !(const NotParser<ParserNodeType>& node) {
        return node.parser();
    }


} //namespace parserlib


#endif //PARSERLIB_NOTPARSER_HPP
