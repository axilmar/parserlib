#ifndef PARSERLIB_ERRORPARSER_HPP
#define PARSERLIB_ERRORPARSER_HPP


#include "SequenceParser.hpp"


namespace parserlib {


    /**
     * Special structure that wraps a parser in order to indicate an error recovery point.
     * @param ParserType type of parser to use for error recovery.
     */
    template <class ParserType> class ErrorRecoveryPoint {
    public:
        /**
         * The constructor.
         * @param parser parser to use for error recovery.
         */
        ErrorRecoveryPoint(const ParserType& parser) : m_parser(parser) {
        }

        /**
         * Returns the parser to be used for error recovery.
         * @return the parser to be used for error recovery.
         */
        const ParserType& parser() const {
            return m_parser;
        }

    private:
        ParserType m_parser;
    };


    /**
     * An error parser.
     * 
     * It is a binary parser: the right-hand-side parser is used for error recovery.
     * 
     * Error recovery can be done automatically via the following construct:
     * 
     *    LHS >> ~RHS
     * 
     * @param LHS left-hand-side parser; type of parser that is on the left side of `operator >>()`.
     * @param RHS right-hand-side parser; type of parser that is on the right side of `operator >>()`; used for error recovery.
     */
    template <class LHS, class RHS> class ErrorParser : public ParserNode<ErrorParser<LHS, RHS>> {
    public:
        /**
         * The constructor.
         * @param lhs left-hand-side parser.
         * @param rhs right-hand-side parser.
         */
        ErrorParser(const LHS& lhs, const RHS& rhs) : m_lhs(lhs), m_rhs(rhs) {
        }

        /**
         * First it invokes the left-hand-side parser, and if that suceeds,
         * then it invokes the right-hand-side parser.
         * 
         * If both parsers suceeds, nothing else happens.
         * 
         * Otherwise, it keeps invoking the right hand parser and incrementing the source code position,
         * until a match is found or the source is exhausted.
         * 
         * If a match is found, then it returns true, and also commits the current error.
         * 
         * The parser state is restored after the invocation of the child parser.
         * @param pc parse context.
         * @return true if parsing succeeds, false otherwise.
         */
        template <class ParseContextType> bool operator ()(ParseContextType& pc) const {
            if (m_lhs(pc)) {
                if (m_rhs(pc)) {
                    return true;
                }
            }

            if (recover(pc)) {
                return true;
            }

            return false;
        }

        /**
         * Same as operator (), but called in a left recursion parsing context.
         * @param pc parse context.
         * @param lrc left recursion context.
         * @return true if parsing succeeds, false otherwise.
         */
        template <class ParseContextType> bool parseLeftRecursionContinuation(ParseContextType& pc, LeftRecursionContext<ParseContextType>& lrc) const {
            if (m_lhs.parseLeftRecursionContinuation(pc, lrc)) {
                if (lrc.continuationResolved()) {
                    if (m_rhs(pc)) {
                        return true;
                    }
                }
                else {
                    if (m_rhs.parseLeftRecursionContinuation(pc, lrc)) {
                        return true;
                    }
                }
            }

            if (recover(pc)) {
                return true;
            }

            return false;
        }

    private:
        LHS m_lhs;
        RHS m_rhs;

        //recover from error
        template <class ParseContextType> bool recover(ParseContextType& pc) const {
            //commit the current errors
            pc.commitErrors();

            //rewind the error state on end; don't record errors while testing for recovery
            const auto errorState = pc.errorState();

            //until the source is exhausted
            while (!pc.sourceEnded()) {

                //if the right-hand-side parser suceeds, then remove the temporary errors
                //and report success
                if (m_rhs(pc)) {
                    pc.setErrorState(errorState);
                    return true;
                }

                //next position since current position failed to parse
                pc.incrementSourcePosition();
            }

            //error; no recovery possible
            pc.setErrorState(errorState);
            return false;
        }
    };


    /**
     * Operator that creates an error recovery point from a parser node.
     * @param node parser node to create an error recovery point from.
     * @return an error recovery point.
     */
    template <class ParserNodeType>
    ErrorRecoveryPoint<ParserNodeType> operator ~(const ParserNode<ParserNodeType>& node) {
        return { static_cast<const ParserNodeType&>(node) };
    }


    /**
     * A sequence operator between a parser node and an error recovery point.
     * @param lhs the left-hand-side parser.
     * @param rhs the right-hand-side parser.
     * @return an error recovery parser.
     */
    template <class LHS, class RHS> 
    ErrorParser<LHS, RHS> operator >> (const ParserNode<LHS>& lhs, const ErrorRecoveryPoint<RHS>& rhs) {
        return { static_cast<const LHS&>(lhs), rhs.parser() };
    }


} //namespace parserlib


#endif //PARSERLIB_ERRORPARSER_HPP
