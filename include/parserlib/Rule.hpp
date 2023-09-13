#ifndef PARSERLIB_RULE_HPP
#define PARSERLIB_RULE_HPP


#include <memory>
#include "ParseContext.hpp"
#include "ParserWrapper.hpp"
#include "RuleReference.hpp"
#include "SequenceParser.hpp"
#include "ChoiceParser.hpp"
#include "Loop0Parser.hpp"
#include "Loop1Parser.hpp"
#include "LoopNParser.hpp"
#include "OptionalParser.hpp"
#include "AndParser.hpp"
#include "NotParser.hpp"
#include "MatchParser.hpp"
#include "TreeMatchParser.hpp"
#include "util.hpp"
#include "ErrorParser.hpp"


namespace parserlib {


    /**
     * An interface to a parser that can be recursive.
     * @param ParseContextType type of context to pass to the parse function.
     */
    template <class ParseContextType = ParseContext<>> class Rule {
    public:
        /**
         * Rule state type.
         */
        using RuleStateType = RuleState<ParseContextType>;

        /**
         * Constructor.
         * Allocates a copy of the given parser on the heap.
         * @param parser parser to copy.
         */
        template <class ParserNodeType> 
        Rule(const ParserNode<ParserNodeType>& parser)
            : m_parser(std::make_shared<ParserWrapper<ParseContextType, ParserNodeType>>(static_cast<const ParserNodeType&>(parser)))
        {
        }
        
        /**
         * Constructor from terminal.
         * @param term terminal.
         */
        template <class TerminalType, std::enable_if_t<!std::is_base_of_v<ParserNodeBase, TerminalType>, int> = 0>
        Rule(const TerminalType& term) : Rule(terminal(term)) {
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
        const Rule<ParseContextType>* this_() const {
            return this;
        }

        /**
         * Returns this ptr.
         * Operator & is reserved for a specific operation of the library.
         * @return this ptr.
         */
        Rule<ParseContextType>* this_() {
            return this;
        }

        /**
         * Invokes the underlying parser.
         * @param pc parse context.
         * @return whatever the underlying parser returns.
         * @exception LeftRecursionException thrown when left recursion is found upon the referenced rule.
         */
        bool operator ()(ParseContextType& pc) const {
            return parse(pc, 
                [](RuleStateType& ruleState) {
                    ruleState.setLeftRecursion(true);
                    return false;
                });
        }

        /**
         * If there is no left recursion at the specific point,
         * then it reverts to normal parsing. Otherwise,
         * it sets the left-recursion context's continuationResolved flag to true,
         * so as that subsequent parsers parse normally and returns true.
         * The object is called to parse within a left recursion parsing context,
         * in order to continue parsing after the non-left recursive part is parsed.
         * @param pc parse context.
         * @param lrc left recursion context.
         * @return true if parsing succeeds, false otherwise.
         */
        bool parseLeftRecursionContinuation(ParseContextType& pc, LeftRecursionContext<ParseContextType>& lrc) const {
            return parse(pc,
                [&](RuleStateType& /*ruleState*/) {
                    lrc.setContinuationResolved(true);
                    return true;
                });
        }

    private:
        const std::shared_ptr<ParserInterface<ParseContextType>> m_parser;

        //parse
        template <class LRF> bool parse(ParseContextType& pc, const LRF& lrf) const {
            //get the state for the rule
            RuleStateType& ruleState = pc.ruleState(*this);

            //check if there is left recursion
            if (ruleState.position() == pc.sourcePosition()) {
                return lrf(ruleState);
            }

            //no left recursion; proceed with normal parsing

            //keep the current state to later restore it
            const RuleStateType prevState = ruleState;

            //at scope exit, restore the rule state
            const ScopeExit scopeExitHandler([&]() { ruleState = prevState; });

            //initialize the rule state for non-left recursive parsing
            ruleState.setPosition(pc.sourcePosition());
            ruleState.setLeftRecursion(false);

            //create a left recursion context, if needed later
            LeftRecursionContext<ParseContextType> lrc(pc.sourcePosition(), pc.matches().size());

            //parse
            const bool result = m_parser->operator()(pc);

            //failure
            if (!result) {
                return false;
            }

            //success

            //if left recursion was detected, parse continuation
            if (ruleState.leftRecursion()) {
                return parseLeftRecursionContinuation(pc, ruleState, lrc);
            }

            return true;
        }

        //parse left recursion continuation
        bool parseLeftRecursionContinuation(ParseContextType& pc, RuleStateType& ruleState, LeftRecursionContext<ParseContextType>& lrc) const {
            while (!pc.sourceEnded()) {
                const auto startPosition = pc.sourcePosition();

                //set the current position so as that more left recursion is found
                ruleState.setPosition(pc.sourcePosition());

                //invoke the parser
                if (!m_parser->parseLeftRecursionContinuation(pc, lrc)) {
                    break;
                }

                //if there is no advance, then no more left recursion continuation parsing can be done
                if (pc.sourcePosition() == startPosition) {
                    break;
                }
            }

            //success
            return true;
        }
    };


    template <class ParseContextType, class R>
    auto operator >> (Rule<ParseContextType>&&, R&&) = delete;


    template <class L, class ParseContextType>
    auto operator >> (L&&, Rule<ParseContextType>&&) = delete;


    /**
     * Sequence between two rules.
     * @param rule1 the 1st rule.
     * @param rule2 the 2nd rule.
     * @return a sequence of the two rules.
     */
    template <class ParseContextType>
    auto operator >> (const Rule<ParseContextType>& rule1, const Rule<ParseContextType>& rule2) {
        return RuleReference<ParseContextType>(rule1) >> RuleReference<ParseContextType>(rule2);
    }


    /**
     * Sequence between a rule and a parser node.
     * @param rule the rule parser.
     * @param node the node parser.
     * @return a sequence of the rule and node.
     */
    template <class ParseContextType, class ParserNodeType>
    auto operator >> (const Rule<ParseContextType>& rule, const ParserNode<ParserNodeType>& node) {
        return RuleReference<ParseContextType>(rule) >> node;
    }


    /**
     * Sequence between a parser node and a rule.
     * @param node the node parser.
     * @param rule the rule parser.
     * @return a sequence of the two nodes.
     */
    template <class ParserNodeType, class ParseContextType>
    auto operator >> (const ParserNode<ParserNodeType>& node, const Rule<ParseContextType>& rule) {
        return node >> RuleReference<ParseContextType>(rule);
    }


    /**
     * Sequence between a rule and a terminal.
     * @param rule the rule parser.
     * @param term the terminal.
     * @return a sequence of the rule and terminal.
     */
    template <class ParseContextType, class TerminalType, std::enable_if_t<!std::is_base_of_v<ParserNodeBase, TerminalType>, int> = 0>
    auto operator >> (const Rule<ParseContextType>& rule, const TerminalType& term) {
        return RuleReference<ParseContextType>(rule) >> terminal(term);
    }


    /**
     * Sequence between a terminal and a rule.
     * @param term the terminal.
     * @param rule the rule parser.
     * @return a sequence of the terminal and rule.
     */
    template <class ParseContextType, class TerminalType, std::enable_if_t<!std::is_base_of_v<ParserNodeBase, TerminalType>, int> = 0>
    auto operator >> (const TerminalType& term, const Rule<ParseContextType>& rule) {
        return terminal(term) >> RuleReference<ParseContextType>(rule);
    }


    template <class ParseContextType, class R>
    auto operator - (Rule<ParseContextType>&&, R&&) = delete;


    template <class L, class ParseContextType>
    auto operator - (L&&, Rule<ParseContextType>&&) = delete;


    /**
     * Subtracts the 2nd rule from the first.
     * @param rule1 the 1st rule.
     * @param rule2 the 2nd rule.
     * @return a sequence of the two rules.
     */
    template <class ParseContextType>
    auto operator - (const Rule<ParseContextType>& rule1, const Rule<ParseContextType>& rule2) {
        return RuleReference<ParseContextType>(rule1) - RuleReference<ParseContextType>(rule2);
    }


    /**
     * Subtracts a parser node from a rule.
     * @param rule the rule parser.
     * @param node the node parser.
     * @return a sequence of the rule and node.
     */
    template <class ParseContextType, class ParserNodeType>
    auto operator - (const Rule<ParseContextType>& rule, const ParserNode<ParserNodeType>& node) {
        return RuleReference<ParseContextType>(rule) - node;
    }


    /**
     * Subtracts a rule from a parser node.
     * @param node the node parser.
     * @param rule the rule parser.
     * @return a sequence of the two nodes.
     */
    template <class ParserNodeType, class ParseContextType>
    auto operator - (const ParserNode<ParserNodeType>& node, const Rule<ParseContextType>& rule) {
        return node - RuleReference<ParseContextType>(rule);
    }


    /**
     * Subtracts a terminal from a rule.
     * @param rule the rule parser.
     * @param term the terminal.
     * @return a sequence of the rule and terminal.
     */
    template <class ParseContextType, class TerminalType, std::enable_if_t<!std::is_base_of_v<ParserNodeBase, TerminalType>, int> = 0>
    auto operator - (const Rule<ParseContextType>& rule, const TerminalType& term) {
        return RuleReference<ParseContextType>(rule) - terminal(term);
    }


    /**
     * Subtracts a rule from a terminal.
     * @param term the terminal.
     * @param rule the rule parser.
     * @return a sequence of the terminal and rule.
     */
    template <class ParseContextType, class TerminalType, std::enable_if_t<!std::is_base_of_v<ParserNodeBase, TerminalType>, int> = 0>
    auto operator - (const TerminalType& term, const Rule<ParseContextType>& rule) {
        return terminal(term) - RuleReference<ParseContextType>(rule);
    }


    template <class ParseContextType, class R>
    auto operator | (Rule<ParseContextType>&&, R&&) = delete;


    template <class L, class ParseContextType>
    auto operator | (L&&, Rule<ParseContextType>&&) = delete;


    /**
     * Choice between two rules.
     * @param rule1 the 1st rule.
     * @param rule2 the 2nd rule.
     * @return a choice of the two rules.
     */
    template <class ParseContextType>
    auto operator | (const Rule<ParseContextType>& rule1, const Rule<ParseContextType>& rule2) {
        return RuleReference<ParseContextType>(rule1) | RuleReference<ParseContextType>(rule2);
    }


    /**
     * Choice between a rule and a parser node.
     * @param rule the rule parser.
     * @param node the node parser.
     * @return a choice of the rule and node.
     */
    template <class ParseContextType, class ParserNodeType>
    auto operator | (const Rule<ParseContextType>& rule, const ParserNode<ParserNodeType>& node) {
        return RuleReference<ParseContextType>(rule) | node;
    }


    /**
     * Choice between a parser node and a rule.
     * @param node the node parser.
     * @param rule the rule parser.
     * @return a choice of the two nodes.
     */
    template <class ParserNodeType, class ParseContextType>
    auto operator | (const ParserNode<ParserNodeType>& node, const Rule<ParseContextType>& rule) {
        return node | RuleReference<ParseContextType>(rule);
    }


    /**
     * Choice between a rule and a terminal.
     * @param rule the rule parser.
     * @param term the terminal.
     * @return a sequence of the rule and terminal.
     */
    template <class ParseContextType, class TerminalType, std::enable_if_t<!std::is_base_of_v<ParserNodeBase, TerminalType>, int> = 0>
    auto operator | (const Rule<ParseContextType>& rule, const TerminalType& term) {
        return RuleReference<ParseContextType>(rule) | terminal(term);
    }


    /**
     * Choice between a terminal and a rule.
     * @param term the terminal.
     * @param rule the rule parser.
     * @return a sequence of the terminal and rule.
     */
    template <class ParseContextType, class TerminalType, std::enable_if_t<!std::is_base_of_v<ParserNodeBase, TerminalType>, int> = 0>
    auto operator | (const TerminalType& term, const Rule<ParseContextType>& rule) {
        return terminal(term) | RuleReference<ParseContextType>(rule);
    }


    template <class ParseContextType>
    auto operator *(Rule<ParseContextType>&&) = delete;


    /**
     * Creates a loop parser out of a rule.
     * @param rule rule.
     * @return the loop parser for the given rule.
     */
    template <class ParseContextType> auto operator *(const Rule<ParseContextType>& rule) {
        return *RuleReference<ParseContextType>(rule);
    }


    template <class ParseContextType>
    auto operator +(Rule<ParseContextType>&&) = delete;


    /**
     * Creates a loop-1 parser out of a rule.
     * @param rule rule.
     * @return the loop-1 parser for the given rule.
     */
    template <class ParseContextType> auto operator +(const Rule<ParseContextType>& rule) {
        return +RuleReference<ParseContextType>(rule);
    }


    template <class ParseContextType>
    auto operator -(Rule<ParseContextType>&&) = delete;


    /**
     * Creates an optional parser out of a rule.
     * @param rule rule.
     * @return the optional parser for the given rule.
     */
    template <class ParseContextType> auto operator -(const Rule<ParseContextType>& rule) {
        return -RuleReference<ParseContextType>(rule);
    }


    template <class ParseContextType>
    auto operator &(Rule<ParseContextType>&&) = delete;


    /**
     * Creates an and parser out of a rule.
     * @param rule rule.
     * @return the and parser for the given rule.
     */
    template <class ParseContextType> auto operator &(const Rule<ParseContextType>& rule) {
        return &RuleReference<ParseContextType>(rule);
    }


    template <class ParseContextType>
    auto operator !(Rule<ParseContextType>&&) = delete;


    /**
     * Creates a not parser out of a rule.
     * @param rule rule.
     * @return the not parser for the given rule.
     */
    template <class ParseContextType> auto operator !(const Rule<ParseContextType>& rule) {
        return !RuleReference<ParseContextType>(rule);
    }


    template <class ParseContextType, class MatchIdType>
    auto operator == (Rule<ParseContextType>&&, const MatchIdType&) = delete;


    /**
     * Operator that allows a match to be inserted into the parser for a rule.
     * @param rule rule to apply the operator to.
     * @param matchId match id.
     * @return a match parser.
     */
    template <class ParseContextType, class MatchIdType>
    MatchParser<RuleReference<ParseContextType>, MatchIdType>
        operator == (const Rule<ParseContextType>& rule, const MatchIdType& matchId) {
        return MatchParser<RuleReference<ParseContextType>, MatchIdType>(RuleReference<ParseContextType>(rule), matchId);
    }


    template <class ParseContextType, class MatchIdType>
    auto operator >= (Rule<ParseContextType>&&, const MatchIdType&) = delete;


    /**
     * Operator that allows a tree match to be inserted into the parser for a rule.
     * @param rule rule to apply the operator to.
     * @param matchId match id.
     * @return a tree match parser.
     */
    template <class ParseContextType, class MatchIdType>
    TreeMatchParser<RuleReference<ParseContextType>, MatchIdType>
        operator >= (const Rule<ParseContextType>& rule, const MatchIdType& matchId) {
        return TreeMatchParser<RuleReference<ParseContextType>, MatchIdType>(RuleReference<ParseContextType>(rule), matchId);
    }


    template <class ParseContextType>
    TreeMatchParser<RuleReference<ParseContextType>, std::string>
        operator >= (const Rule<ParseContextType>& rule, const char* matchId) {
        return TreeMatchParser<RuleReference<ParseContextType>, std::string>(RuleReference<ParseContextType>(rule), matchId);
    }


    template <class ParseContextType>
    TreeMatchParser<RuleReference<ParseContextType>, std::wstring>
        operator >= (const Rule<ParseContextType>& rule, const wchar_t* matchId) {
        return TreeMatchParser<RuleReference<ParseContextType>, std::wstring>(RuleReference<ParseContextType>(rule), matchId);
    }


    template <class ParseContextType>
    TreeMatchParser<RuleReference<ParseContextType>, std::u16string>
        operator >= (const Rule<ParseContextType>& rule, const char16_t* matchId) {
        return TreeMatchParser<RuleReference<ParseContextType>, std::u16string>(RuleReference<ParseContextType>(rule), matchId);
    }


    template <class ParseContextType>
    TreeMatchParser<RuleReference<ParseContextType>, std::u32string>
        operator >= (const Rule<ParseContextType>& rule, const char32_t* matchId) {
        return TreeMatchParser<RuleReference<ParseContextType>, std::u32string>(RuleReference<ParseContextType>(rule), matchId);
    }


    template <class ParseContextType>
    auto operator * (size_t loopCount, Rule<ParseContextType>&& rule) = delete;


    /**
     * Loop-n-count over a rule.
     * @param loopCount number of loops; cannot be 0.
     * @param rule rule to repeat.
     * @return loop-n-parser instance.
     * @exception std::invalid_argument thrown if loop count is 0.
     */
    template <class ParseContextType>
    LoopNParser<RuleReference<ParseContextType>>
    operator * (size_t loopCount, const Rule<ParseContextType>& rule) {
        return { loopCount, RuleReference<ParseContextType>(rule) };
    }


    template <class ParseContextType>
    auto operator ~(Rule<ParseContextType>&&) = delete;


    /**
     * Operator that creates an error recovery point from a rule.
     * @param rule rule to create an error recovery point from.
     * @return an error recovery point.
     */
    template <class ParseContextType>
    ErrorRecoveryPoint<RuleReference<ParseContextType>> operator ~(const Rule<ParseContextType>& rule) {
        return { RuleReference<ParseContextType>(rule) };
    }


    template <class ParseContextType, class RHS>
    auto operator >> (Rule<ParseContextType>&& lhs, const ErrorRecoveryPoint<RHS>& rhs) = delete;


    /**
     * A sequence operator between a rule and an error recovery point.
     * @param lhs the left-hand-side rule.
     * @param rhs the right-hand-side parser.
     * @return an error recovery parser.
     */
    template <class ParseContextType, class RHS>
    ErrorParser<RuleReference<ParseContextType>, RHS> operator >> (const Rule<ParseContextType>& lhs, const ErrorRecoveryPoint<RHS>& rhs) {
        return { RuleReference<ParseContextType>(lhs), rhs.parser() };
    }


} //namespace parserlib


#endif //PARSERLIB_RULE_HPP
