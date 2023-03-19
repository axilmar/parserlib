#ifndef PARSERLIB_RULE_HPP
#define PARSERLIB_RULE_HPP


#include <memory>
#include "ParserWrapper.hpp"
#include "RuleReference.hpp"
#include "SequenceParser.hpp"
#include "ChoiceParser.hpp"
#include "LoopParser.hpp"
#include "Loop1Parser.hpp"
#include "OptionalParser.hpp"
#include "AndParser.hpp"
#include "NotParser.hpp"
#include "Match.hpp"


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
         * Invokes the underlying parser.
         * @param pc parse context.
         * @return whatever the underlying parser returns.
         */
        bool operator ()(ParseContextType& pc) const {
            return m_parser->operator ()(pc);
        }

    private:
        std::shared_ptr<ParserInterface<ParseContextType>> m_parser;
    };


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
     * Deleted in order to forbid temporaries. 
     */
    template <class ParseContextType, class T> int operator >> (Rule<ParseContextType>&& rule, T&&) = delete;


    /**
     * Deleted in order to forbid temporaries.
     */
    template <class ParseContextType, class T> int operator >> (const Rule<ParseContextType>&&, T&&) = delete;


    /**
     * Deleted in order to forbid temporaries.
     */
    template <class ParseContextType, class T> int operator >> (T&&, Rule<ParseContextType>&&) = delete;


    /**
     * Deleted in order to forbid temporaries.
     */
    template <class ParseContextType, class T> int operator >> (T&&, const Rule<ParseContextType>&&) = delete;


    /**
     * Choice between two rules.
     * @param rule1 the 1st rule.
     * @param rule2 the 2nd rule.
     * @return a sequence of the two rules.
     */
    template <class ParseContextType>
    auto operator | (const Rule<ParseContextType>& rule1, const Rule<ParseContextType>& rule2) {
        return RuleReference<ParseContextType>(rule1) | RuleReference<ParseContextType>(rule2);
    }


    /**
     * Choice between a rule and a parser node.
     * @param rule the rule parser.
     * @param node the node parser.
     * @return a sequence of the rule and node.
     */
    template <class ParseContextType, class ParserNodeType>
    auto operator | (const Rule<ParseContextType>& rule, const ParserNode<ParserNodeType>& node) {
        return RuleReference<ParseContextType>(rule) | node;
    }


    /**
     * Choice between a parser node and a rule.
     * @param node the node parser.
     * @param rule the rule parser.
     * @return a sequence of the two nodes.
     */
    template <class ParserNodeType, class ParseContextType>
    auto operator | (const ParserNode<ParserNodeType>& node, const Rule<ParseContextType>& rule) {
        return node | RuleReference<ParseContextType>(rule);
    }


    /**
     * Deleted in order to forbid temporaries.
     */
    template <class ParseContextType, class T> int operator | (Rule<ParseContextType>&& rule, T&&) = delete;


    /**
     * Deleted in order to forbid temporaries.
     */
    template <class ParseContextType, class T> int operator | (const Rule<ParseContextType>&&, T&&) = delete;


    /**
     * Deleted in order to forbid temporaries.
     */
    template <class ParseContextType, class T> int operator | (T&&, Rule<ParseContextType>&&) = delete;


    /**
     * Deleted in order to forbid temporaries.
     */
    template <class ParseContextType, class T> int operator | (T&&, const Rule<ParseContextType>&&) = delete;


    /**
     * Creates a loop parser out of a rule.
     * @param rule rule.
     * @return the loop parser for the given rule.
     */
    template <class ParseContextType> auto operator *(const Rule<ParseContextType>& rule) {
        return *RuleReference<ParseContextType>(rule);
    }


    /**
     * Deleted in order to forbid temporaries.
     */
    template <class ParseContextType> int operator *(Rule<ParseContextType>&&) = delete;


    /**
     * Deleted in order to forbid temporaries.
     */
    template <class ParseContextType> int operator *(const Rule<ParseContextType>&&) = delete;


    /**
     * Creates a loop-1 parser out of a rule.
     * @param rule rule.
     * @return the loop-1 parser for the given rule.
     */
    template <class ParseContextType> auto operator +(const Rule<ParseContextType>& rule) {
        return +RuleReference<ParseContextType>(rule);
    }


    /**
     * Deleted in order to forbid temporaries.
     */
    template <class ParseContextType> int operator +(Rule<ParseContextType>&&) = delete;


    /**
     * Deleted in order to forbid temporaries.
     */
    template <class ParseContextType> int operator +(const Rule<ParseContextType>&&) = delete;


    /**
     * Creates an optional parser out of a rule.
     * @param rule rule.
     * @return the optional parser for the given rule.
     */
    template <class ParseContextType> auto operator -(const Rule<ParseContextType>& rule) {
        return -RuleReference<ParseContextType>(rule);
    }


    /**
     * Deleted in order to forbid temporaries.
     */
    template <class ParseContextType> int operator -(Rule<ParseContextType>&&) = delete;


    /**
     * Deleted in order to forbid temporaries.
     */
    template <class ParseContextType> int operator -(const Rule<ParseContextType>&&) = delete;


    /**
     * Creates an and parser out of a rule.
     * @param rule rule.
     * @return the and parser for the given rule.
     */
    template <class ParseContextType> auto operator &(const Rule<ParseContextType>& rule) {
        return &RuleReference<ParseContextType>(rule);
    }


    /**
     * Deleted in order to forbid temporaries.
     */
    template <class ParseContextType> int operator &(Rule<ParseContextType>&&) = delete;


    /**
     * Deleted in order to forbid temporaries.
     */
    template <class ParseContextType> int operator &(const Rule<ParseContextType>&&) = delete;


    /**
     * Creates a not parser out of a rule.
     * @param rule rule.
     * @return the not parser for the given rule.
     */
    template <class ParseContextType> auto operator !(const Rule<ParseContextType>& rule) {
        return !RuleReference<ParseContextType>(rule);
    }


    /**
     * Deleted in order to forbid temporaries.
     */
    template <class ParseContextType> int operator !(Rule<ParseContextType>&&) = delete;


    /**
     * Deleted in order to forbid temporaries.
     */
    template <class ParseContextType> int operator !(const Rule<ParseContextType>&&) = delete;


    /**
     * Operator that allows a match to be inserted into the parser for a rule.
     * @param rule rule to apply the operator to.
     * @param matchId match id.
     * @return a match parser.
     */
    template <class ParseContextType, class MatchIdType>
    Match<RuleReference<ParseContextType>, MatchIdType>
        operator >>= (const Rule<ParseContextType>& rule, const MatchIdType& matchId) {
        return Match<RuleReference<ParseContextType>, MatchIdType>(RuleReference<ParseContextType>(rule), matchId);
    }


    /**
     * Deleted in order to forbid temporaries.
     */
    template <class ParseContextType, class T> int operator >>= (Rule<ParseContextType>&&, T&&) = delete;


    /**
     * Deleted in order to forbid temporaries.
     */
    template <class ParseContextType, class T> int operator >>= (const Rule<ParseContextType>&&, T&&) = delete;


} //namespace parserlib


#endif //PARSERLIB_RULE_HPP
