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
            return parse(pc, [&]() { return m_child(pc); });
        }

        /**
         * Invokes the child parser once. If that succeeds,
         * then it invokes the child parser repeatedly, until no more parsing can succeed.
         * The object is called to parse within a left recursion parsing context,
         * in order to continue parsing after the non-left recursive part is parsed.
         * @param pc parse context.
         * @param lrc left recursion context.
         * @return true if parsing succeeds, false otherwise.
         */
        template <class ParseContextType> bool parseLeftRecursionContinuation(ParseContextType& pc, LeftRecursionContext<ParseContextType>& lrc) const {
            return parse(pc, [&]() { return m_child.parseLeftRecursionContinuation(pc, lrc); });
        }

    private:
        ParserNodeType m_child;

        template <class ParseContextType, class PF> bool parse(ParseContextType& pc, const PF& pf) const {
            //parse the child once to define the result
            {
                const auto startPosition = pc.sourcePosition();

                if (!pf()) {
                    return false;
                }

                //if no input was consumed, stop in order to avoid an infinite loop
                if (startPosition == pc.sourcePosition()) {
                    return false;
                }
            }

            const auto errorState = pc.errorState();

            //parse loop; normal function, since advance was made
            while (true) {
                const auto startPosition = pc.sourcePosition();

                //if no more parsing possible, stop
                if (!m_child(pc)) {                    
                    break;
                }

                //if no input was consumed, stop in order to avoid an infinite loop
                if (startPosition == pc.sourcePosition()) {
                    break;
                }
            }

            pc.setErrorState(errorState);

            //success
            return true;
        }
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
