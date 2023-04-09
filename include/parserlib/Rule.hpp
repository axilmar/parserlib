#ifndef PARSERLIB_RULE_HPP
#define PARSERLIB_RULE_HPP


#include <memory>
#include <optional>
#include "ParserWrapper.hpp"
#include "RuleReference.hpp"
#include "SequenceParser.hpp"
#include "ChoiceParser.hpp"
#include "Loop0Parser.hpp"
#include "Loop1Parser.hpp"
#include "OptionalParser.hpp"
#include "AndParser.hpp"
#include "NotParser.hpp"
#include "Match.hpp"
#include "TreeMatch.hpp"
#include "LeftRecursionException.hpp"
#include "LeftRecursionAccepted.hpp"
#include "util.hpp"


namespace parserlib {


    /**
     * An interface to a parser that can be recursive.
     * @param ParseContextType type of context to pass to the parse function.
     */
    template <class ParseContextType = ParseContext<>> class Rule {
    public:
        /**
         * Constructor.
         * Allocates a copy of the given parser on the heap.
         * @param parser parser to copy.
         */
        template <class ParserType> Rule(const ParserType& parser)
            : m_parser(std::make_shared<ParserWrapper<ParseContextType, ParserType>>(parser))
        {
        }

        /**
         * Returns the parser.
         * @return the parser.
         */
        const std::shared_ptr<ParserInterface<ParseContextType>>& parser() const {
            return m_parser;
        }

        /**
         * Returns this ptr.
         * Operator & is reserved for a specific operation of the library.
         * @return this ptr.
         */
        const Rule<ParseContextType>* ptr() const {
            return this;
        }

        /**
         * Returns this ptr.
         * Operator & is reserved for a specific operation of the library.
         * @return this ptr.
         */
        Rule<ParseContextType>* ptr() {
            return this;
        }

        /**
         * Compares pointer addresses.
         * Operator & is reserved for a specific operation of the library.
         * @param rule rule to check against.
         * @return true if this and given rule are the same object.
         */
        bool isSame(const Rule<ParseContextType>* rule) const {
            return this == rule;
        }

        /**
         * Compares pointer addresses.
         * Operator & is reserved for a specific operation of the library.
         * @param rule rule to check against.
         * @return true if this and given rule are the same object.
         */
        bool isSame(const Rule<ParseContextType>& rule) const {
            return isSame(rule.ptr());
        }

        /**
         * Invokes the underlying parser.
         * @param pc parse context.
         * @return whatever the underlying parser returns.
         * @exception LeftRecursionException thrown when left recursion is found upon the referenced rule.
         */
        bool operator ()(ParseContextType& pc) {
            switch (tryParse(pc)) {
                case Failure:
                    return false;

                case Success:
                    return true;

                case LeftRecursion:
                    throw LeftRecursionException<ParseContextType>(*this);
            }

            throw std::logic_error("Rule: operator (): invalid state.");
        }

        /**
         * Parses a terminal under left recursion.
         * @param pc parse context.
         * @return true if a terminal was parsed, false otherwise.
         */
        bool parseLeftRecursionTerminal(ParseContextType& pc) {
            switch (tryParse(pc)) {
                case Failure:
                    return false;

                case Success:
                    return true;

                case LeftRecursion:
                    return false;
            }

            throw std::logic_error("Rule: parseLeftRecursionTerminal: invalid state.");
        }

        /**
         * Parses a left recursion continuation.
         * @param pc parse context.
         * @return true on success, false otherwise.
         */
        bool parseLeftRecursionContinuation(ParseContextType& pc) {
            switch (tryParse(pc)) {
                case Failure:
                    return false;

                case Success:
                    return true;

                case LeftRecursion:
                    throw LeftRecursionAccepted();
            }

            throw std::logic_error("Rule: parseLeftRecursionContinuation: invalid state.");
        }

    private:
        enum ParseResult {
            Failure,
            Success,
            LeftRecursion
        };

        const std::shared_ptr<ParserInterface<ParseContextType>> m_parser;
        std::optional<typename ParseContextType::Position> m_parsePosition;

        //try parsing
        ParseResult tryParse(ParseContextType& pc) {
            //if same position, return left recursion
            if (m_parsePosition == pc.sourcePosition()) {
                return LeftRecursion;
            }

            const auto startPosition = m_parsePosition;

            RAII restoreParsePosition = [&]() {
                m_parsePosition = startPosition;
            };

            m_parsePosition = pc.sourcePosition();

            //catch left recursion if thrown from normal parsing
            try {
                return m_parser->operator ()(pc) ? Success : Failure;
            }

            //if this object threw the exception, then handle the left recursion,
            //else propagate the exception to the rule that threw it
            catch (const LeftRecursionException<ParseContextType>& lre) {
                if (lre.rule().isSame(this)) {
                    return handleLeftRecursion(pc) ? Success : Failure;
                }
                throw lre;
            }
            
            //if this is thrown, it means there was no sequence to handle it,
            //and therefore nothing was parsed; return failure
            catch (LeftRecursionAccepted) {
                return Failure;
            }
        }

        //handle left recursion
        bool handleLeftRecursion(ParseContextType& pc) {
            m_parsePosition = pc.sourcePosition();

            //try to parse a terminal under left recursion; 
            //if it fails, fail the parsing
            try {
                if (!m_parser->parseLeftRecursionTerminal(pc)) {
                    return false;
                }
            }
            catch (const LeftRecursionException<ParseContextType>&) {
                return false;
            }

            //parse the left recursion continuation, until no more parsing possible
            while (!pc.sourceEnded()) {
                const auto startPosition = pc.sourcePosition();

                m_parsePosition = pc.sourcePosition();

                try {
                    //parse continuation; stop on failure or if no progress could be made
                    if (!m_parser->parseLeftRecursionContinuation(pc) || pc.sourcePosition() == startPosition) {
                        break;
                    }
                }

                //no child parser could parse left recursion; stop the continuation
                catch (const LeftRecursionException<ParseContextType>&) {
                    break;
                }
            }

            //success
            return true;
        }
    };


    /**
     * Sequence between two rules.
     * @param rule1 the 1st rule.
     * @param rule2 the 2nd rule.
     * @return a sequence of the two rules.
     */
    template <class ParseContextType>
    auto operator >> (Rule<ParseContextType>& rule1, Rule<ParseContextType>& rule2) {
        return RuleReference<ParseContextType>(rule1) >> RuleReference<ParseContextType>(rule2);
    }


    /**
     * Sequence between a rule and a parser node.
     * @param rule the rule parser.
     * @param node the node parser.
     * @return a sequence of the rule and node.
     */
    template <class ParseContextType, class ParserNodeType>
    auto operator >> (Rule<ParseContextType>& rule, const ParserNode<ParserNodeType>& node) {
        return RuleReference<ParseContextType>(rule) >> node;
    }


    /**
     * Sequence between a parser node and a rule.
     * @param node the node parser.
     * @param rule the rule parser.
     * @return a sequence of the two nodes.
     */
    template <class ParserNodeType, class ParseContextType>
    auto operator >> (const ParserNode<ParserNodeType>& node, Rule<ParseContextType>& rule) {
        return node >> RuleReference<ParseContextType>(rule);
    }


    /**
     * Choice between two rules.
     * @param rule1 the 1st rule.
     * @param rule2 the 2nd rule.
     * @return a sequence of the two rules.
     */
    template <class ParseContextType>
    auto operator | (Rule<ParseContextType>& rule1, Rule<ParseContextType>& rule2) {
        return RuleReference<ParseContextType>(rule1) | RuleReference<ParseContextType>(rule2);
    }


    /**
     * Choice between a rule and a parser node.
     * @param rule the rule parser.
     * @param node the node parser.
     * @return a sequence of the rule and node.
     */
    template <class ParseContextType, class ParserNodeType>
    auto operator | (Rule<ParseContextType>& rule, const ParserNode<ParserNodeType>& node) {
        return RuleReference<ParseContextType>(rule) | node;
    }


    /**
     * Choice between a parser node and a rule.
     * @param node the node parser.
     * @param rule the rule parser.
     * @return a sequence of the two nodes.
     */
    template <class ParserNodeType, class ParseContextType>
    auto operator | (const ParserNode<ParserNodeType>& node, Rule<ParseContextType>& rule) {
        return node | RuleReference<ParseContextType>(rule);
    }


    /**
     * Creates a loop parser out of a rule.
     * @param rule rule.
     * @return the loop parser for the given rule.
     */
    template <class ParseContextType> auto operator *(Rule<ParseContextType>& rule) {
        return *RuleReference<ParseContextType>(rule);
    }


    /**
     * Creates a loop-1 parser out of a rule.
     * @param rule rule.
     * @return the loop-1 parser for the given rule.
     */
    template <class ParseContextType> auto operator +(Rule<ParseContextType>& rule) {
        return +RuleReference<ParseContextType>(rule);
    }


    /**
     * Creates an optional parser out of a rule.
     * @param rule rule.
     * @return the optional parser for the given rule.
     */
    template <class ParseContextType> auto operator -(Rule<ParseContextType>& rule) {
        return -RuleReference<ParseContextType>(rule);
    }


    /**
     * Creates an and parser out of a rule.
     * @param rule rule.
     * @return the and parser for the given rule.
     */
    template <class ParseContextType> auto operator &(Rule<ParseContextType>& rule) {
        return &RuleReference<ParseContextType>(rule);
    }


    /**
     * Creates a not parser out of a rule.
     * @param rule rule.
     * @return the not parser for the given rule.
     */
    template <class ParseContextType> auto operator !(Rule<ParseContextType>& rule) {
        return !RuleReference<ParseContextType>(rule);
    }


    /**
     * Operator that allows a match to be inserted into the parser for a rule.
     * @param rule rule to apply the operator to.
     * @param matchId match id.
     * @return a match parser.
     */
    template <class ParseContextType, class MatchIdType>
    Match<RuleReference<ParseContextType>, MatchIdType>
        operator == (Rule<ParseContextType>& rule, const MatchIdType& matchId) {
        return Match<RuleReference<ParseContextType>, MatchIdType>(RuleReference<ParseContextType>(rule), matchId);
    }


    /**
     * Operator that allows a tree match to be inserted into the parser for a rule.
     * @param rule rule to apply the operator to.
     * @param matchId match id.
     * @return a tree match parser.
     */
    template <class ParseContextType, class MatchIdType>
    TreeMatch<RuleReference<ParseContextType>, MatchIdType>
        operator >= (Rule<ParseContextType>& rule, const MatchIdType& matchId) {
        return TreeMatch<RuleReference<ParseContextType>, MatchIdType>(RuleReference<ParseContextType>(rule), matchId);
    }


} //namespace parserlib


#endif //PARSERLIB_RULE_HPP
