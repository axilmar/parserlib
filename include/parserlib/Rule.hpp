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
#include "OptionalParser.hpp"
#include "AndParser.hpp"
#include "NotParser.hpp"
#include "Match.hpp"
#include "TreeMatch.hpp"
#include "TerminalParser.hpp"
#include "TerminalStringParser.hpp"
#include "util.hpp"


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
        template <class ParserType> Rule(const ParserType& parser)
            : m_parser(std::make_shared<ParserWrapper<ParseContextType, ParserType>>(parser))
        {
        }

        /**
         * Constructor from char.
         * @param ch character.
         */
        Rule(char ch) : Rule(TerminalParser<char>(ch)) {
        }

        /**
         * Constructor from string.
         * @param str string.
         */
        Rule(const char* str) : Rule(TerminalStringParser<char>(str)) {
        }

        /**
         * Constructor from wchar_t.
         * @param ch character.
         */
        Rule(wchar_t ch) : Rule(TerminalParser<wchar_t>(ch)) {
        }

        /**
         * Constructor from string.
         * @param str string.
         */
        Rule(const wchar_t* str) : Rule(TerminalStringParser<wchar_t>(str)) {
        }

        /**
         * Constructor from char16_t.
         * @param ch character.
         */
        Rule(char16_t ch) : Rule(TerminalParser<char16_t>(ch)) {
        }

        /**
         * Constructor from string.
         * @param str string.
         */
        Rule(const char16_t* str) : Rule(TerminalStringParser<char16_t>(str)) {
        }

        /**
         * Constructor from char32_t.
         * @param ch character.
         */
        Rule(char32_t ch) : Rule(TerminalParser<char32_t>(ch)) {
        }

        /**
         * Constructor from string.
         * @param str string.
         */
        Rule(const char32_t* str) : Rule(TerminalStringParser<char32_t>(str)) {
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

        bool parseLeftRecursionBase(ParseContextType& pc) const {
            return parse(pc,
                [](RuleStateType& ruleState) {
                    return false;
                });
        }

        bool parseLeftRecursionContinuation(ParseContextType& pc, LeftRecursionContext<ParseContextType>& lrc) const {
            return parse(pc,
                [&](RuleStateType& ruleState) {
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
     * Sequence between a rule and a character.
     * @param rule the rule parser.
     * @param ch the character.
     * @return a sequence of the rule and a character.
     */
    template <class ParseContextType>
    auto operator >> (const Rule<ParseContextType>& rule, char ch) {
        return operator >> (rule, TerminalParser<char>(ch));
    }


    /**
     * Sequence between a rule and a string.
     * @param rule the rule parser.
     * @param str the string.
     * @return a sequence of the rule and a string.
     */
    template <class ParseContextType>
    auto operator >> (const Rule<ParseContextType>& rule, const char* str) {
        return rule >> TerminalStringParser<char>(str);
    }


    /**
     * Sequence between a rule and a wide character.
     * @param rule the rule parser.
     * @param ch the character.
     * @return a sequence of the rule and a character.
     */
    template <class ParseContextType>
    auto operator >> (const Rule<ParseContextType>& rule, wchar_t ch) {
        return rule >> TerminalParser<wchar_t>(ch);
    }


    /**
     * Sequence between a rule and a wide character string.
     * @param rule the rule parser.
     * @param str the string.
     * @return a sequence of the rule and a string.
     */
    template <class ParseContextType>
    auto operator >> (const Rule<ParseContextType>& rule, const wchar_t* str) {
        return rule >> TerminalStringParser<wchar_t>(str);
    }


    /**
     * Sequence between a rule and a 16-bit character.
     * @param rule the rule parser.
     * @param ch the character.
     * @return a sequence of the rule and a character.
     */
    template <class ParseContextType>
    auto operator >> (const Rule<ParseContextType>& rule, char16_t ch) {
        return rule >> TerminalParser<char16_t>(ch);
    }


    /**
     * Sequence between a rule and a 16-bit character string.
     * @param rule the rule parser.
     * @param str the string.
     * @return a sequence of the rule and a string.
     */
    template <class ParseContextType>
    auto operator >> (const Rule<ParseContextType>& rule, const char16_t* str) {
        return rule >> TerminalStringParser<char16_t>(str);
    }


    /**
     * Sequence between a rule and a 32-bit character.
     * @param rule the rule parser.
     * @param ch the character.
     * @return a sequence of the rule and a character.
     */
    template <class ParseContextType>
    auto operator >> (const Rule<ParseContextType>& rule, char32_t ch) {
        return rule >> TerminalParser<char32_t>(ch);
    }


    /**
     * Sequence between a rule and a 32-bit character string.
     * @param rule the rule parser.
     * @param str the string.
     * @return a sequence of the rule and a string.
     */
    template <class ParseContextType>
    auto operator >> (const Rule<ParseContextType>& rule, const char32_t* str) {
        return rule >> TerminalStringParser<char32_t>(str);
    }


    /**
     * Sequence between a char and a rule.
     * @param ch the character.
     * @param rule the rule parser.
     * @return a sequence of a char terminal and a rule.
     */
    template <class ParseContextType>
    auto operator >> (char ch, const Rule<ParseContextType>& rule) {
        return TerminalParser<char>(ch) >> RuleReference<ParseContextType>(rule);
    }


    /**
     * Sequence between a char string and a rule.
     * @param str the string.
     * @param rule the rule parser.
     * @return a sequence of a string terminal and a rule.
     */
    template <class ParseContextType>
    auto operator >> (const char* str, const Rule<ParseContextType>& rule) {
        return TerminalStringParser<char>(str) >> RuleReference<ParseContextType>(rule);
    }


    /**
     * Sequence between a wchar_t and a rule.
     * @param ch the character.
     * @param rule the rule parser.
     * @return a sequence of a wchar_t terminal and a rule.
     */
    template <class ParseContextType>
    auto operator >> (wchar_t ch, const Rule<ParseContextType>& rule) {
        return TerminalParser<wchar_t>(ch) >> RuleReference<ParseContextType>(rule);
    }


    /**
     * Sequence between a wchar_t string and a rule.
     * @param str the string.
     * @param rule the rule parser.
     * @return a sequence of a string terminal and a rule.
     */
    template <class ParseContextType>
    auto operator >> (const wchar_t* str, const Rule<ParseContextType>& rule) {
        return TerminalStringParser<wchar_t>(str) >> RuleReference<ParseContextType>(rule);
    }


    /**
     * Sequence between a char16_t and a rule.
     * @param ch the character.
     * @param rule the rule parser.
     * @return a sequence of a char16_t terminal and a rule.
     */
    template <class ParseContextType>
    auto operator >> (char16_t ch, const Rule<ParseContextType>& rule) {
        return TerminalParser<char16_t>(ch) >> RuleReference<ParseContextType>(rule);
    }


    /**
     * Sequence between a char16_t string and a rule.
     * @param str the string.
     * @param rule the rule parser.
     * @return a sequence of a string terminal and a rule.
     */
    template <class ParseContextType>
    auto operator >> (const char16_t* str, const Rule<ParseContextType>& rule) {
        return TerminalStringParser<char16_t>(str) >> RuleReference<ParseContextType>(rule);
    }


    /**
     * Sequence between a char32_t and a rule.
     * @param ch the character.
     * @param rule the rule parser.
     * @return a sequence of a char32_t terminal and a rule.
     */
    template <class ParseContextType>
    auto operator >> (char32_t ch, const Rule<ParseContextType>& rule) {
        return TerminalParser<char32_t>(ch) >> RuleReference<ParseContextType>(rule);
    }


    /**
     * Sequence between a char32_t string and a rule.
     * @param str the string.
     * @param rule the rule parser.
     * @return a sequence of a string terminal and a rule.
     */
    template <class ParseContextType>
    auto operator >> (const char32_t* str, const Rule<ParseContextType>& rule) {
        return TerminalStringParser<char32_t>(str) >> RuleReference<ParseContextType>(rule);
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
     * Choice between a rule and a char.
     * @param rule the rule parser.
     * @param ch the character.
     * @return a choice of the rule and char.
     */
    template <class ParseContextType>
    auto operator | (const Rule<ParseContextType>& rule, char ch) {
        return RuleReference<ParseContextType>(rule) | TerminalParser<char>(ch);
    }


    /**
     * Choice between a rule and a char string.
     * @param rule the rule parser.
     * @param str the string.
     * @return a choice of the rule and string.
     */
    template <class ParseContextType>
    auto operator | (const Rule<ParseContextType>& rule, const char* str) {
        return RuleReference<ParseContextType>(rule) | TerminalStringParser<char>(str);
    }


    /**
     * Choice between a rule and a wchar_t.
     * @param rule the rule parser.
     * @param ch the character.
     * @return a choice of the rule and wchar_t.
     */
    template <class ParseContextType>
    auto operator | (const Rule<ParseContextType>& rule, wchar_t ch) {
        return RuleReference<ParseContextType>(rule) | TerminalParser<wchar_t>(ch);
    }


    /**
     * Choice between a rule and a wchar_t string.
     * @param rule the rule parser.
     * @param str the string.
     * @return a choice of the rule and string.
     */
    template <class ParseContextType>
    auto operator | (const Rule<ParseContextType>& rule, const wchar_t* str) {
        return RuleReference<ParseContextType>(rule) | TerminalStringParser<wchar_t>(str);
    }


    /**
     * Choice between a rule and a char16_t.
     * @param rule the rule parser.
     * @param ch the character.
     * @return a choice of the rule and char16_t.
     */
    template <class ParseContextType>
    auto operator | (const Rule<ParseContextType>& rule, char16_t ch) {
        return RuleReference<ParseContextType>(rule) | TerminalParser<char16_t>(ch);
    }


    /**
     * Choice between a rule and a char16_t string.
     * @param rule the rule parser.
     * @param str the string.
     * @return a choice of the rule and string.
     */
    template <class ParseContextType>
    auto operator | (const Rule<ParseContextType>& rule, const char16_t* str) {
        return RuleReference<ParseContextType>(rule) | TerminalStringParser<char16_t>(str);
    }


    /**
     * Choice between a rule and a char32_t.
     * @param rule the rule parser.
     * @param ch the character.
     * @return a choice of the rule and char32_t.
     */
    template <class ParseContextType>
    auto operator | (const Rule<ParseContextType>& rule, char32_t ch) {
        return RuleReference<ParseContextType>(rule) | TerminalParser<char32_t>(ch);
    }


    /**
     * Choice between a rule and a char32_t string.
     * @param rule the rule parser.
     * @param str the string.
     * @return a choice of the rule and string.
     */
    template <class ParseContextType>
    auto operator | (const Rule<ParseContextType>& rule, const char32_t* str) {
        return RuleReference<ParseContextType>(rule) | TerminalStringParser<char32_t>(str);
    }


    /**
     * Choice between a char and a rule.
     * @param ch the char.
     * @param rule the rule parser.
     * @return a choice between a char and a rule.
     */
    template <class ParseContextType>
    auto operator | (char ch, const Rule<ParseContextType>& rule) {
        return TerminalParser<char>(ch) | RuleReference<ParseContextType>(rule);
    }


    /**
     * Choice between a char string and a rule.
     * @param str the char string.
     * @param rule the rule parser.
     * @return a choice between a char string and a rule.
     */
    template <class ParseContextType>
    auto operator | (const char* str, const Rule<ParseContextType>& rule) {
        return TerminalStringParser<char>(str) | RuleReference<ParseContextType>(rule);
    }


    /**
     * Choice between a wchar_t and a rule.
     * @param ch the wchar_t.
     * @param rule the rule parser.
     * @return a choice between a wchar_t and a rule.
     */
    template <class ParseContextType>
    auto operator | (wchar_t ch, const Rule<ParseContextType>& rule) {
        return TerminalParser<wchar_t>(ch) | RuleReference<ParseContextType>(rule);
    }


    /**
     * Choice between a wchar_t string and a rule.
     * @param str the wchar_t string.
     * @param rule the rule parser.
     * @return a choice between a wchar_t string and a rule.
     */
    template <class ParseContextType>
    auto operator | (const wchar_t* str, const Rule<ParseContextType>& rule) {
        return TerminalStringParser<wchar_t>(str) | RuleReference<ParseContextType>(rule);
    }


    /**
     * Choice between a char16_t and a rule.
     * @param ch the char16_t.
     * @param rule the rule parser.
     * @return a choice between a char16_t and a rule.
     */
    template <class ParseContextType>
    auto operator | (char16_t ch, const Rule<ParseContextType>& rule) {
        return TerminalParser<char16_t>(ch) | RuleReference<ParseContextType>(rule);
    }


    /**
     * Choice between a char16_t string and a rule.
     * @param str the char16_t string.
     * @param rule the rule parser.
     * @return a choice between a char16_t string and a rule.
     */
    template <class ParseContextType>
    auto operator | (const char16_t* str, const Rule<ParseContextType>& rule) {
        return TerminalStringParser<char16_t>(str) | RuleReference<ParseContextType>(rule);
    }


    /**
     * Choice between a char32_t and a rule.
     * @param ch the char32_t.
     * @param rule the rule parser.
     * @return a choice between a char32_t and a rule.
     */
    template <class ParseContextType>
    auto operator | (char32_t ch, const Rule<ParseContextType>& rule) {
        return TerminalParser<char32_t>(ch) | RuleReference<ParseContextType>(rule);
    }


    /**
     * Choice between a char32_t string and a rule.
     * @param str the char32_t string.
     * @param rule the rule parser.
     * @return a choice between a char32_t string and a rule.
     */
    template <class ParseContextType>
    auto operator | (const char32_t* str, const Rule<ParseContextType>& rule) {
        return TerminalStringParser<char32_t>(str) | RuleReference<ParseContextType>(rule);
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
    Match<RuleReference<ParseContextType>, MatchIdType>
        operator == (const Rule<ParseContextType>& rule, const MatchIdType& matchId) {
        return Match<RuleReference<ParseContextType>, MatchIdType>(RuleReference<ParseContextType>(rule), matchId);
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
    TreeMatch<RuleReference<ParseContextType>, MatchIdType>
        operator >= (const Rule<ParseContextType>& rule, const MatchIdType& matchId) {
        return TreeMatch<RuleReference<ParseContextType>, MatchIdType>(RuleReference<ParseContextType>(rule), matchId);
    }


    template <class ParseContextType>
    TreeMatch<RuleReference<ParseContextType>, std::string>
        operator >= (const Rule<ParseContextType>& rule, const char* matchId) {
        return TreeMatch<RuleReference<ParseContextType>, std::string>(RuleReference<ParseContextType>(rule), matchId);
    }


    template <class ParseContextType>
    TreeMatch<RuleReference<ParseContextType>, std::wstring>
        operator >= (const Rule<ParseContextType>& rule, const wchar_t* matchId) {
        return TreeMatch<RuleReference<ParseContextType>, std::wstring>(RuleReference<ParseContextType>(rule), matchId);
    }


    template <class ParseContextType>
    TreeMatch<RuleReference<ParseContextType>, std::u16string>
        operator >= (const Rule<ParseContextType>& rule, const char16_t* matchId) {
        return TreeMatch<RuleReference<ParseContextType>, std::u16string>(RuleReference<ParseContextType>(rule), matchId);
    }


    template <class ParseContextType>
    TreeMatch<RuleReference<ParseContextType>, std::u32string>
        operator >= (const Rule<ParseContextType>& rule, const char32_t* matchId) {
        return TreeMatch<RuleReference<ParseContextType>, std::u32string>(RuleReference<ParseContextType>(rule), matchId);
    }


} //namespace parserlib


#endif //PARSERLIB_RULE_HPP
