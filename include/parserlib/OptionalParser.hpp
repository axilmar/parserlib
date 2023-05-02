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
            const auto errorState = pc.errorState();
            m_child(pc);
            pc.setErrorState(errorState);
            return true;
        }

        /**
         * Invokes the child parser, then returns true.
         * The object is called to parse within a left recursion parsing context,
         * in order to continue parsing after the non-left recursive part is parsed.
         * @param pc parse context.
         * @param lrc left recursion context.
         * @return true if parsing succeeds, false otherwise.
         */
        template <class ParseContextType> bool parseLeftRecursionContinuation(ParseContextType& pc, LeftRecursionContext<ParseContextType>& lrc) const {
            const auto errorState = pc.errorState();
            m_child.parseLeftRecursionContinuation(pc, lrc);
            pc.setErrorState(errorState);
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
