#ifndef PARSERLIB_LOOP0PARSER_HPP
#define PARSERLIB_LOOP0PARSER_HPP


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
                const auto startPosition = pc.sourcePosition();

                //if no more parsing possible, stop
                if (!m_child(pc)) {
                    break;
                }

                //if no advance was made, stop in order to avoid infinite an loop
                if (pc.sourcePosition() == startPosition) {
                    break;
                }
            }

            return true;
        }

        /**
         * Parses a terminal under left recursion.
         * @param pc parse context.
         * @return true if a terminal was parsed, false otherwise.
         */
        template <class ParseContextType> bool parseLeftRecursionTerminal(ParseContextType& pc) const {
            const auto startPosition = pc.sourcePosition();

            //parse once to establish if parsing consumes input
            if (!m_child.parseLeftRecursionTerminal(pc)) {
                return false;
            }

            //if no input was consumed, stop in order to avoid an infinite loop
            if (pc.sourcePosition() == startPosition) {
                return true;
            }

            //parse loop; normal parsing since at least one terminal was consumed
            while (true) {
                const auto startPosition = pc.sourcePosition();

                //if no more parsing possible, stop
                if (!m_child(pc)) {
                    break;
                }

                //if no advance was made, stop in order to avoid infinite an loop
                if (pc.sourcePosition() == startPosition) {
                    break;
                }
            }

            return true;
        }

        /**
         * Parses a left recursion continuation.
         * @param pc parse context.
         * @return true on success, false otherwise.
         */
        template <class ParseContextType> bool parseLeftRecursionContinuation(ParseContextType& pc) const {
            const auto startPosition = pc.sourcePosition();

            //parse once to establish if parsing consumes input
            if (!m_child.parseLeftRecursionContinuation(pc)) {
                return false;
            }

            //if no input was consumed, stop in order to avoid an infinite loop
            if (pc.sourcePosition() == startPosition) {
                return true;
            }

            //parse loop; normal parsing since at least one terminal was consumed
            while (true) {
                const auto startPosition = pc.sourcePosition();

                //if no more parsing possible, stop
                if (!m_child(pc)) {
                    break;
                }

                //if no advance was made, stop in order to avoid infinite an loop
                if (pc.sourcePosition() == startPosition) {
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


#endif //PARSERLIB_LOOP0PARSER_HPP
