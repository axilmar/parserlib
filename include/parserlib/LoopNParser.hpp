#ifndef PARSERLIB_LOOPNPARSER_HPP
#define PARSERLIB_LOOPNPARSER_HPP


#include <type_traits>
#include <stdexcept>
#include "ParserNode.hpp"
#include "TerminalParser.hpp"
#include "TerminalStringParser.hpp"


namespace parserlib {


    /**
     * A parser that invokes another parser specific times.
     * @param ParserNodeType the parser to invoke in a loop.
     */
    template <class ParserNodeType> class LoopNParser : public ParserNode<LoopNParser<ParserNodeType>> {
    public:
        /**
         * The default constructor.
         * @param loopCount number of loops; cannot be 0.
         * @param child child parser to invoke in a loop.
         * @exception std::invalid_argument thrown if loop count is 0.
         */
        LoopNParser(size_t loopCount, const ParserNodeType& child) :
            m_loopCount(loopCount), m_child(child)
        {
            if (!loopCount) {
                throw std::invalid_argument("Loop count cannot be 0.");
            }
        }

        /**
         * Returns the loop count.
         * @return the loop count.
         */
        size_t loopCount() const {
            return m_loopCount;
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
            return parse(pc, [&]() { return m_child(pc); });
        }

        /**
         * Invokes the child parser repeatedly, until no more parsing can succeed.
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
        const size_t m_loopCount;
        const ParserNodeType m_child;

        template <class ParseContextType, class PF> bool parse(ParseContextType& pc, const PF& pf) const {
            const auto initialState = pc.state();

            //parse once with the given function
            {
                const auto startPosition = pc.sourcePosition();

                //if no more parsing possible, stop
                if (!pf()) {
                    return false;
                }

                //if no advance was made, stop in order to avoid infinite an loop
                if (pc.sourcePosition() == startPosition) {
                    return true;
                }
            }

            //since parsing suceeded, parse with normal function from now on
            for (size_t loopCount = 1; loopCount < m_loopCount; ++loopCount) {
                const auto startPosition = pc.sourcePosition();

                //if no more parsing possible, stop
                if (!m_child(pc)) {
                    pc.setState(initialState);
                    return false;
                }

                //if no advance was made, stop in order to avoid infinite an loop
                if (pc.sourcePosition() == startPosition) {
                    break;
                }
            }

            return true;
        }
    };


    /**
     * Binary multiplication operator that allows the definition of a N-time loop over a parser node.
     * @param loopCount number of loops; cannot be 0.
     * @param node parse node to repeat.
     * @return loop-n-parser instance.
     * @exception std::invalid_argument thrown if loop count is 0.
     */
    template <class ParseNodeType>
    LoopNParser<ParseNodeType>
        operator * (size_t loopCount, const ParserNode<ParseNodeType>& node) {
        return { loopCount, static_cast<const ParseNodeType&>(node) };
    }


    /**
     * Optimization for looping over a loop-n-count.
     * @param loopCount number of loops; cannot be 0.
     * @param node parse node to repeat.
     * @return loop-n-parser instance which loops over the given parser loopCount * node.loopCount() times.
     * @exception std::invalid_argument thrown if loop count is 0.
     */
    template <class ParseNodeType>
    LoopNParser<ParseNodeType>
        operator * (size_t loopCount, const ParserNode<LoopNParser<ParseNodeType>>& node) {
        const LoopNParser<ParseNodeType>& loopNode = static_cast<const LoopNParser<ParseNodeType>&>(node);
        return { loopCount * loopNode.loopCount(), loopNode.child() };
    }


    /**
     * Loop-n-count over a terminal.
     * @param loopCount number of loops; cannot be 0.
     * @param term terminal to repeat.
     * @return loop-n-parser instance.
     * @exception std::invalid_argument thrown if loop count is 0.
     */
    template <class TerminalType, std::enable_if_t<!std::is_base_of_v<ParserNodeBase, TerminalType>, int> = 0>
    LoopNParser<decltype(terminal(TerminalType()))>
        operator * (size_t loopCount, const TerminalType& term) {
        return { loopCount, terminal(term) };
    }


} //namespace parserlib


#endif //PARSERLIB_LOOPNPARSER_HPP
