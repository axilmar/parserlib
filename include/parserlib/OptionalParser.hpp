#ifndef PARSERLIB_OPTIONALPARSER_HPP
#define PARSERLIB_OPTIONALPARSER_HPP


#include "ParserNode.hpp"


namespace parserlib {


    /**
     * A parser that invokes another parser and always returns true.
     * @param ParserNodeType the parser to invoke.
     */
    template <class ParserNodeType> class OptionalParser : public ParserNode<OptionalParser<ParserNodeType>> {
    public:
        /**
         * The default constructor.
         * @param child child parser to invoke.
         */
        OptionalParser(const ParserNodeType& child) : m_child(child) {
        }

        /**
         * Returns the parser to invoke.
         * @return the parser to invoke.
         */
        const ParserNodeType& child() const {
            return m_child;
        }

        /**
         * Invokes the child parser, then returns true.
         * @param pc parse context.
         * @return always true.
         */
        template <class ParseContextType> bool operator ()(ParseContextType& pc) const {
            m_child(pc);
            return true;
        }

        /**
         * Parses a terminal under left recursion.
         * @param pc parse context.
         * @return true if a terminal was parsed, false otherwise.
         */
        template <class ParseContextType> bool parseLeftRecursionTerminal(ParseContextType& pc) const {
            m_child.parseLeftRecursionTerminal(pc);
            return true;
        }

        /**
         * Parses a left recursion continuation.
         * @param pc parse context.
         * @return true on success, false otherwise.
         */
        template <class ParseContextType> bool parseLeftRecursionContinuation(ParseContextType& pc) const {
            m_child.parseLeftRecursionContinuation(pc);
            return true;
        }

    private:
        const ParserNodeType m_child;
    };


    /**
     * Converts a parser node into an optional parser.
     * @param node parser node to convert to optional.
     * @return an optional parser node.
     */
    template <class ParserNodeType> 
    OptionalParser<ParserNodeType> 
    operator -(const ParserNode<ParserNodeType>& node) {
        return OptionalParser<ParserNodeType>(static_cast<const ParserNodeType&>(node));
    }


    /**
     * Avoids converting an optional parser node into another optional parser.
     * @param optional optional parser node.
     * @return the given optional parser node.
     */
    template <class ParserNodeType>
    const OptionalParser<ParserNodeType>&
    operator -(const OptionalParser<ParserNodeType>& optional) {
        return optional;
    }


} //namespace parserlib


#endif //PARSERLIB_OPTIONALPARSER_HPP
