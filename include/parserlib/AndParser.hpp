#ifndef PARSERLIB_ANDPARSER_HPP
#define PARSERLIB_ANDPARSER_HPP


#include "ParserNode.hpp"


namespace parserlib {


    /**
     * A parser that invokes another parser to validate a grammar; does not consume input.
     * @param ParserNodeType the parser to invoke.
     */
    template <class ParserNodeType> class AndParser : public ParserNode<AndParser<ParserNodeType>> {
    public:
        /**
         * The default constructor.
         * @param child child parser to invoke.
         */
        AndParser(const ParserNodeType& child) : m_child(child) {
        }

        /**
         * Returns the parser to invoke.
         * @return the parser to invoke.
         */
        const ParserNodeType& child() const {
            return m_child;
        }

        /**
         * Invokes the child parser, then returns the result.
         * The parser state is restored after the invocation of the child parser.
         * @param pc parse context.
         * @return true if parsing succeeds, false otherwise.
         */
        template <class ParseContextType> bool operator ()(ParseContextType& pc) const {
            return parse(pc, [&]() { return m_child(pc); });
        }

        template <class ParseContextType> bool parseLeftRecursionBase(ParseContextType& pc) const {
            return parse(pc, [&]() { return m_child.parseLeftRecursionBase(pc); });
        }

        template <class ParseContextType> bool parseLeftRecursionContinuation(ParseContextType& pc, LeftRecursionContext<ParseContextType>& lrc) const {
            return parse(pc, [&]() { return m_child.parseLeftRecursionContinuation(pc, lrc); });
        }

    private:
        const ParserNodeType m_child;

        template <class ParseContextType, class PF> bool parse(ParseContextType& pc, const PF& pf) const {
            const auto state = pc.state();
            const bool result = pf();
            pc.setState(state);
            return result;
        }
    };


    /**
     * Converts a parser node into an and parser.
     * @param node parser node to convert to and.
     * @return an and parser node.
     */
    template <class ParserNodeType> 
    AndParser<ParserNodeType> 
    operator &(const ParserNode<ParserNodeType>& node) {
        return AndParser<ParserNodeType>(static_cast<const ParserNodeType&>(node));
    }


    /**
     * Avoids converting an and parser node into another and parser.
     * @param node and parser node.
     * @return the given and parser node.
     */
    template <class ParserNodeType>
    const AndParser<ParserNodeType>&
    operator &(const AndParser<ParserNodeType>& node) {
        return node;
    }


} //namespace parserlib


#endif //PARSERLIB_ANDPARSER_HPP
