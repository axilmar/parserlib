#ifndef PARSERLIB_RULE_HPP
#define PARSERLIB_RULE_HPP


#include <memory>
#include <stdexcept>
#include "ParseContext.hpp"
#include "RuleReference.hpp"
#include "ZeroOrMoreParser.hpp"
#include "OneOrMoreParser.hpp"
#include "OptionalParser.hpp"
#include "NotParser.hpp"
#include "AndParser.hpp"
#include "SequenceParser.hpp"
#include "ChoiceParser.hpp"
#include "MatchParser.hpp"
#include "ErrorResumeParser.hpp"


namespace parserlib {


    /**
     * A rule is a grammar element that can be used recursively.
     * 
     * Rules must live either as global variables or members of objects.
     * Rules are always managed via rule references.
     * 
     * @param ParseContext type of parse context to use for rules.
     * 
     *  Required because in order to allow a rule to be used recursively,
     *  the parsing expression is put inside an interface.
     */
    template <class ParseContext = ParseContext<>>
    class Rule {
    public:
        /**
         * Type of parse context.
         */
        typedef ParseContext ParseContext;

        /**
         * Makes this parser a zero-or-more parser.
         * @return a zero-or-more parser that calls this parser as a child.
         */
        ZeroOrMoreParser<RuleReference<ParseContext>> operator *() {
            return ZeroOrMoreParser<RuleReference<ParseContext>>(self());
        }

        /**
         * Makes this parser an one-or-more parser.
         * @return an one-or-more parser that calls this parser as a child.
         */
        OneOrMoreParser<RuleReference<ParseContext>> operator +() {
            return OneOrMoreParser<RuleReference<ParseContext>>(self());
        }

        /**
         * Makes this parser optional.
         * @return an optional parser that calls this parser as a child.
         */
        OptionalParser<RuleReference<ParseContext>> operator -() {
            return OptionalParser<RuleReference<ParseContext>>(self());
        }

        /**
         * Makes this parser be used as a logical NOT predicate.
         * @return a logical NOT parser that calls this parser as a child.
         */
        NotParser<RuleReference<ParseContext>> operator !() {
            return NotParser<RuleReference<ParseContext>>(self());
        }

        /**
         * Makes this parser be used as a logical AND predicate.
         * @return a logical AND parser that calls this parser as a child.
         */
        AndParser<RuleReference<ParseContext>> operator &() {
            return AndParser<RuleReference<ParseContext>>(self());
        }

        /**
         * Makes this parser an error resume point.
         * @return an error resume point.
         */
        ErrorResumePoint<RuleReference<ParseContext>> operator ~() {
            return ErrorResumePoint<RuleReference<ParseContext>>(self());
        }

        /**
         * Returns a reference to this rule.
         * @return reference to this rule.
         */
        const RuleReference<ParseContext> self() {
            return RuleReference<ParseContext>(*this);
        }

        /**
         * The default constructor.
         * An empty rule is created.
         */
        Rule() {
        }

        /**
         * Copying is forbidden due to internal unique onwership (via std::unique_ptr of the internal parsing expression.
         */
        Rule(const Rule&) = delete;

        /**
         * The move constructor.
         * @param r the rule to be emptied after the call.
         */
        Rule(Rule&& r)
            : m_expression(std::move(r.m_expression))
        {
        }

        /**
         * Wraps the given parsing expression into a rule.
         * @param expression expression to wrap.
         */
        template <class ParseExpression, std::enable_if_t<!std::is_same_v<Rule<ParseContext>, std::decay_t<ParseExpression>>, bool> = true>
        Rule(ParseExpression&& expression)
            : m_expression(std::make_unique<ParseExpressionImpl<ParseExpression>>(std::move(expression)))
        {
        }

        /**
         * Copying is forbidden due to internal unique onwership (via std::unique_ptr of the internal parsing expression.
         */
        Rule& operator = (const Rule&) = delete;

        /**
         * The move assignment operator.
         * @param r the rule to be emptied after the call.
         * @return reference to this.
         */
        Rule& operator = (Rule&& r) {
            m_expression = std::move(r.m_expression);
            return *this;
        }

        /**
         * Wraps the given parsing expression into a rule.
         * @param expression expression to wrap.
         * @return reference to this.
         */
        template <class ParseExpression, std::enable_if_t<!std::is_same_v<Rule<ParseContext>, std::decay_t<ParseExpression>>, bool> = true>
        Rule& operator = (ParseExpression&& expression) {
            m_expression = std::make_unique<ParseExpressionImpl<ParseExpression>>(std::move(expression));
            return *this;
        }

        /**
         * Parses the input.
         * 
         * It uses the facilities provided by the given parse context to handle left recursion.
         * 
         * @param pc parse context to use for parsing.
         * 
         * @return true if it succeeds, false otherwise.
         */
        bool parse(ParseContext& pc) {
            bool result;

            if (!pc.isLeftRecursiveRule(this)) {
                switch (pc.getLeftRecursionRuleState(this)) {
                    case LeftRecursion::State::Init:
                        return parseInitState(pc);

                    case LeftRecursion::State::Reject:
                        pc.setLeftRecursionRuleState(this, LeftRecursion::State::Init);
                        result = parseInitState(pc);
                        pc.setLeftRecursionRuleState(this, LeftRecursion::State::Reject);
                        return result;

                    case LeftRecursion::State::Accept:
                        pc.setLeftRecursionRuleState(this, LeftRecursion::State::Init);
                        result = parseInitState(pc);
                        pc.setLeftRecursionRuleState(this, LeftRecursion::State::Accept);
                        return result;
                }
            }

            else {
                switch (pc.getLeftRecursionRuleState(this)) {
                    case LeftRecursion::State::Init:
                        throw LeftRecursion::Init();

                    case LeftRecursion::State::Reject:
                        throw LeftRecursion::Reject();

                    case LeftRecursion::State::Accept:
                        throw LeftRecursion::Accept();
                }
            }

            throw std::logic_error("Invalid left recursion state");
        }

    private:
        class ParseExpressionInterface {
        public:
            virtual ~ParseExpressionInterface() {
            }

            virtual bool parse(ParseContext& pc) = 0;
        };

        template <class ParseExpression> 
        class ParseExpressionImpl : public ParseExpressionInterface {
        public:
            ParseExpressionImpl(ParseExpression&& expression)
                : m_expression(std::move(expression))
            {
            }

            bool parse(ParseContext& pc) override {
                return m_expression.parse(pc);
            }

        private:
            ParseExpression m_expression;
        };

        std::unique_ptr<ParseExpressionInterface> m_expression;

        template <class ParseContext>
        bool _parse(ParseContext& pc) {
            bool result;
            pc.pushRulePosition(this);
            try {
                result = m_expression->parse(pc);
            }
            catch (...) {
                pc.popRulePosition(this);
                throw;
            }
            pc.popRulePosition(this);
            return result;
        }

        template <class ParseContext>
        bool parseInitState(ParseContext& pc) {
            try {
                return _parse(pc);
            }
            catch (LeftRecursion::Init) {
                bool result;

                const auto leftRecursionStartPosition = pc.getCurrentPosition();
                const size_t leftRecursionStartMatchIndex = pc.getMatches().size();

                pc.setLeftRecursionRuleState(this, LeftRecursion::State::Reject);
                try {
                    result = _parse(pc);
                }
                catch (LeftRecursion::Reject) {
                    pc.setLeftRecursionRuleState(this, LeftRecursion::State::Init);
                    return false;
                }
                if (!result) {
                    pc.setLeftRecursionRuleState(this, LeftRecursion::State::Init);
                    return false;
                }

                pc.pushLeftRecursionMatchState(leftRecursionStartPosition, leftRecursionStartMatchIndex, pc.getCurrentPosition());

                pc.setLeftRecursionRuleState(this, LeftRecursion::State::Accept);
                while (!pc.isEndPosition()) {
                    try {
                        result = _parse(pc);
                    }
                    catch (LeftRecursion::Accept) {
                        pc.setLeftRecursionRuleState(this, LeftRecursion::State::Init);
                        pc.popLeftRecursionMatchState();
                        return false;
                    }
                    if (!result) {
                        break;
                    }
                };

                pc.setLeftRecursionRuleState(this, LeftRecursion::State::Init);
                pc.popLeftRecursionMatchState();
                return true;
            }
        }
    };


    /**
     * Creates a sequence of the given parsers.
     * @param left the left parser/element.
     * @param right the right parser/element.
     * @return a sequence parser for the given left and right parsers/elements.
     */
    template <class ParseContext>
    SequenceParser<RuleReference<ParseContext>, RuleReference<ParseContext>> 
        operator >> (Rule<ParseContext>& left, Rule<ParseContext>& right)
    {
        return SequenceParser<RuleReference<ParseContext>, RuleReference<ParseContext>>(
            RuleReference<ParseContext>(left), RuleReference<ParseContext>(right));
    }


    /**
     * Creates a sequence of the given parsers.
     * @param left the left parser/element.
     * @param right the right parser/element.
     * @return a sequence parser for the given left and right parsers/elements.
     */
    template <class ParseContext, class Right>
    SequenceParser<RuleReference<ParseContext>, Right>
        operator >> (Rule<ParseContext>& left, const Parser<Right>& right)
    {
        return SequenceParser<RuleReference<ParseContext>, Right>(
            RuleReference<ParseContext>(left), right.self());
    }


    /**
     * Creates a sequence of the given parsers.
     * @param left the left parser/element.
     * @param right the right parser/element.
     * @return a sequence parser for the given left and right parsers/elements.
     */
    template <class ParseContext, class Right, std::enable_if_t<!std::is_base_of_v<Parser<Right>, Right>, bool> = true>
    SequenceParser<RuleReference<ParseContext>, TerminalParser<Right>>
        operator >> (Rule<ParseContext>& left, const Right& right)
    {
        return SequenceParser<RuleReference<ParseContext>, TerminalParser<Right>>(
            RuleReference<ParseContext>(left), TerminalParser<Right>(right));
    }


    /**
     * Creates a sequence of the given parsers.
     * @param left the left parser/element.
     * @param right the right parser/element.
     * @return a sequence parser for the given left and right parsers/elements.
     */
    template <class ParseContext, class Right>
    SequenceParser<RuleReference<ParseContext>, TerminalStringParser<Right>>
        operator >> (Rule<ParseContext>& left, const Right* right)
    {
        return SequenceParser<RuleReference<ParseContext>, TerminalStringParser<Right>>(
            RuleReference<ParseContext>(left), TerminalStringParser<Right>(right));
    }


    /**
     * Creates a sequence of the given parsers.
     * @param left the left parser/element.
     * @param right the right parser/element.
     * @return a sequence parser for the given left and right parsers/elements.
     */
    template <class ParseContext, class CharT, class Traits = std::char_traits<CharT>, class Allocator>
    SequenceParser<RuleReference<ParseContext>, TerminalStringParser<CharT>>
        operator >> (Rule<ParseContext>& left, const std::basic_string<CharT, Traits, Allocator>& right)
    {
        return SequenceParser<RuleReference<ParseContext>, TerminalStringParser<CharT>>(
            RuleReference<ParseContext>(left), TerminalStringParser<CharT>(right));
    }


    /**
     * Creates a sequence of the given parsers.
     * @param left the left parser/element.
     * @param right the right parser/element.
     * @return a sequence parser for the given left and right parsers/elements.
     */
    template <class Left, class ParseContext>
    SequenceParser<Left, RuleReference<ParseContext>>
        operator >> (const Parser<Left>& left, Rule<ParseContext>& right)
    {
        return SequenceParser<Left, RuleReference<ParseContext>>(
            left.self(), RuleReference<ParseContext>(right));
    }


    /**
     * Creates a sequence of the given parsers.
     * @param left the left parser/element.
     * @param right the right parser/element.
     * @return a sequence parser for the given left and right parsers/elements.
     */
    template <class Left, class ParseContext, std::enable_if_t<!std::is_base_of_v<Parser<Left>, Left>, bool> = true>
    SequenceParser<TerminalParser<Left>, RuleReference<ParseContext>>
        operator >> (const Left& left, Rule<ParseContext>& right)
    {
        return SequenceParser<TerminalParser<Left>, RuleReference<ParseContext>>(
            TerminalParser<Left>(left), RuleReference<ParseContext>(right));
    }


    /**
     * Creates a sequence of the given parsers.
     * @param left the left parser/element.
     * @param right the right parser/element.
     * @return a sequence parser for the given left and right parsers/elements.
     */
    template <class Left, class ParseContext>
    SequenceParser<TerminalStringParser<Left>, RuleReference<ParseContext>>
        operator >> (const Left* left, Rule<ParseContext>& right)
    {
        return SequenceParser<TerminalStringParser<Left>, RuleReference<ParseContext>>(
            TerminalStringParser<Left>(left), RuleReference<ParseContext>(right));
    }


    /**
     * Creates a sequence of the given parsers.
     * @param left the left parser/element.
     * @param right the right parser/element.
     * @return a sequence parser for the given left and right parsers/elements.
     */
    template <class ParseContext, class CharT, class Traits = std::char_traits<CharT>, class Allocator>
    SequenceParser<TerminalStringParser<CharT>, RuleReference<ParseContext>>
        operator >> (const std::basic_string<CharT, Traits, Allocator>& left, Rule<ParseContext>& right)
    {
        return SequenceParser<TerminalStringParser<CharT>, RuleReference<ParseContext>>(
            TerminalStringParser<CharT>(left), RuleReference<ParseContext>(right));
    }


    /**
     * Creates a choice of the given parsers.
     * @param left the left parser/element.
     * @param right the right parser/element.
     * @return a choice parser for the given left and right parsers/elements.
     */
    template <class ParseContext>
    ChoiceParser<RuleReference<ParseContext>, RuleReference<ParseContext>>
        operator | (Rule<ParseContext>& left, Rule<ParseContext>& right)
    {
        return ChoiceParser<RuleReference<ParseContext>, RuleReference<ParseContext>>(
            RuleReference<ParseContext>(left), RuleReference<ParseContext>(right));
    }


    /**
     * Creates a choice of the given parsers.
     * @param left the left parser/element.
     * @param right the right parser/element.
     * @return a choice parser for the given left and right parsers/elements.
     */
    template <class ParseContext, class Right>
    ChoiceParser<RuleReference<ParseContext>, Right>
        operator | (Rule<ParseContext>& left, const Parser<Right>& right)
    {
        return ChoiceParser<RuleReference<ParseContext>, Right>(
            RuleReference<ParseContext>(left), right.self());
    }


    /**
     * Creates a choice of the given parsers.
     * @param left the left parser/element.
     * @param right the right parser/element.
     * @return a choice parser for the given left and right parsers/elements.
     */
    template <class ParseContext, class Right, std::enable_if_t<!std::is_base_of_v<Parser<Right>, Right>, bool> = true>
    ChoiceParser<RuleReference<ParseContext>, TerminalParser<Right>>
        operator | (Rule<ParseContext>& left, const Right& right)
    {
        return ChoiceParser<RuleReference<ParseContext>, TerminalParser<Right>>(
            RuleReference<ParseContext>(left), TerminalParser<Right>(right));
    }


    /**
     * Creates a choice of the given parsers.
     * @param left the left parser/element.
     * @param right the right parser/element.
     * @return a choice parser for the given left and right parsers/elements.
     */
    template <class ParseContext, class Right>
    ChoiceParser<RuleReference<ParseContext>, TerminalStringParser<Right>>
        operator | (Rule<ParseContext>& left, const Right* right)
    {
        return ChoiceParser<RuleReference<ParseContext>, TerminalStringParser<Right>>(
            RuleReference<ParseContext>(left), TerminalStringParser<Right>(right));
    }


    /**
     * Creates a choice of the given parsers.
     * @param left the left parser/element.
     * @param right the right parser/element.
     * @return a choice parser for the given left and right parsers/elements.
     */
    template <class ParseContext, class CharT, class Traits = std::char_traits<CharT>, class Allocator>
    ChoiceParser<RuleReference<ParseContext>, TerminalStringParser<CharT>>
        operator | (Rule<ParseContext>& left, const std::basic_string<CharT, Traits, Allocator>& right)
    {
        return ChoiceParser<RuleReference<ParseContext>, TerminalStringParser<CharT>>(
            RuleReference<ParseContext>(left), TerminalStringParser<CharT>(right));
    }


    /**
     * Creates a choice of the given parsers.
     * @param left the left parser/element.
     * @param right the right parser/element.
     * @return a choice parser for the given left and right parsers/elements.
     */
    template <class Left, class ParseContext>
    ChoiceParser<Left, RuleReference<ParseContext>>
        operator | (const Parser<Left>& left, Rule<ParseContext>& right)
    {
        return ChoiceParser<Left, RuleReference<ParseContext>>(
            left.self(), RuleReference<ParseContext>(right));
    }


    /**
     * Creates a choice of the given parsers.
     * @param left the left parser/element.
     * @param right the right parser/element.
     * @return a choice parser for the given left and right parsers/elements.
     */
    template <class Left, class ParseContext, std::enable_if_t<!std::is_base_of_v<Parser<Left>, Left>, bool> = true>
    ChoiceParser<TerminalParser<Left>, RuleReference<ParseContext>>
        operator | (const Left& left, Rule<ParseContext>& right)
    {
        return ChoiceParser<TerminalParser<Left>, RuleReference<ParseContext>>(
            TerminalParser<Left>(left), RuleReference<ParseContext>(right));
    }


    /**
     * Creates a choice of the given parsers.
     * @param left the left parser/element.
     * @param right the right parser/element.
     * @return a choice parser for the given left and right parsers/elements.
     */
    template <class Left, class ParseContext>
    ChoiceParser<TerminalStringParser<Left>, RuleReference<ParseContext>>
        operator | (const Left* left, Rule<ParseContext>& right)
    {
        return ChoiceParser<TerminalStringParser<Left>, RuleReference<ParseContext>>(
            TerminalStringParser<Left>(left), RuleReference<ParseContext>(right));
    }


    /**
     * Creates a choice of the given parsers.
     * @param left the left parser/element.
     * @param right the right parser/element.
     * @return a choice parser for the given left and right parsers/elements.
     */
    template <class ParseContext, class CharT, class Traits = std::char_traits<CharT>, class Allocator>
    ChoiceParser<TerminalStringParser<CharT>, RuleReference<ParseContext>>
        operator | (const std::basic_string<CharT, Traits, Allocator>& left, Rule<ParseContext>& right)
    {
        return ChoiceParser<TerminalStringParser<CharT>, RuleReference<ParseContext>>(
            TerminalStringParser<CharT>(left), RuleReference<ParseContext>(right));
    }


    /**
     * Creates a match parser for the given parser.
     * @param parser parser to create a match parser for.
     * @param matchId id of the match.
     * @return a match parser for the given parser.
     */
    template <class ParseContext, class MatchId>
    MatchParser<RuleReference<ParseContext>, MatchId> operator ->* (Rule<ParseContext>& rule, const MatchId& matchId) {
        return MatchParser<RuleReference<ParseContext>, MatchId>(RuleReference<ParseContext>(rule), matchId);
    }


    /**
     * Creates a sequence of the given parsers,
     * where the right parser is placed into a logical NOT parser,
     * and followed by the left parser.
     * @param left the left parser/element.
     * @param right the right parser/element.
     * @return a sequence parser for the given left and right parsers/elements.
     */
    template <class ParseContext>
    auto operator - (Rule<ParseContext>& left, Rule<ParseContext>& right) {
            return !RuleReference<ParseContext>(right) >> RuleReference<ParseContext>(left);
    }


    /**
     * Creates a sequence of the given parsers,
     * where the right parser is placed into a logical NOT parser,
     * and followed by the left parser.
     * @param left the left parser/element.
     * @param right the right parser/element.
     * @return a sequence parser for the given left and right parsers/elements.
     */
    template <class ParseContext, class Right>
    auto operator - (Rule<ParseContext>& left, const Parser<Right>& right) {
        return !right.self() >> RuleReference<ParseContext>(left);
    }


    /**
     * Creates a sequence of the given parsers,
     * where the right parser is placed into a logical NOT parser,
     * and followed by the left parser.
     * @param left the left parser/element.
     * @param right the right parser/element.
     * @return a sequence parser for the given left and right parsers/elements.
     */
    template <class ParseContext, class Right, std::enable_if_t<!std::is_base_of_v<Parser<Right>, Right>, bool> = true>
    auto operator - (Rule<ParseContext>& left, const Right& right) {
        return !TerminalParser<Right>(right) >> RuleReference<ParseContext>(left);
    }


    /**
     * Creates a sequence of the given parsers,
     * where the right parser is placed into a logical NOT parser,
     * and followed by the left parser.
     * @param left the left parser/element.
     * @param right the right parser/element.
     * @return a sequence parser for the given left and right parsers/elements.
     */
    template <class ParseContext, class Right>
    auto operator - (Rule<ParseContext>& left, const Right* right) {
        return !TerminalStringParser<Right>(right) >> RuleReference<ParseContext>(left);
    }


    /**
     * Creates a sequence of the given parsers,
     * where the right parser is placed into a logical NOT parser,
     * and followed by the left parser.
     * @param left the left parser/element.
     * @param right the right parser/element.
     * @return a sequence parser for the given left and right parsers/elements.
     */
    template <class ParseContext, class CharT, class Traits = std::char_traits<CharT>, class Allocator>
    auto operator - (Rule<ParseContext>& left, const std::basic_string<CharT, Traits, Allocator>& right) {
        return !TerminalStringParser<CharT>(right) >> RuleReference<ParseContext>(left);
    }


    /**
     * Creates a sequence of the given parsers,
     * where the right parser is placed into a logical NOT parser,
     * and followed by the left parser.
     * @param left the left parser/element.
     * @param right the right parser/element.
     * @return a sequence parser for the given left and right parsers/elements.
     */
    template <class Left, class ParseContext>
    auto operator - (const Parser<Left>& left, Rule<ParseContext>& right) {
        return !RuleReference<ParseContext>(right) >> left.self();
    }


    /**
     * Creates a sequence of the given parsers,
     * where the right parser is placed into a logical NOT parser,
     * and followed by the left parser.
     * @param left the left parser/element.
     * @param right the right parser/element.
     * @return a sequence parser for the given left and right parsers/elements.
     */
    template <class Left, class ParseContext, std::enable_if_t<!std::is_base_of_v<Parser<Left>, Left>, bool> = true>
    auto operator - (const Left& left, Rule<ParseContext>& right) {
        return !RuleReference<ParseContext>(right) >> TerminalParser<Left>(left);
    }


    /**
     * Creates a sequence of the given parsers,
     * where the right parser is placed into a logical NOT parser,
     * and followed by the left parser.
     * @param left the left parser/element.
     * @param right the right parser/element.
     * @return a sequence parser for the given left and right parsers/elements.
     */
    template <class Left, class ParseContext>
    auto operator - (const Left* left, Rule<ParseContext>& right) {
        return !RuleReference<ParseContext>(right) >> TerminalStringParser<Left>(left);
    }


    /**
     * Creates a sequence of the given parsers,
     * where the right parser is placed into a logical NOT parser,
     * and followed by the left parser.
     * @param left the left parser/element.
     * @param right the right parser/element.
     * @return a sequence parser for the given left and right parsers/elements.
     */
    template <class ParseContext, class CharT, class Traits = std::char_traits<CharT>, class Allocator>
    auto operator - (const std::basic_string<CharT, Traits, Allocator>& left, Rule<ParseContext>& right) {
        return !RuleReference<ParseContext>(right) >> TerminalStringParser<CharT>(left);
    }


} //namespace parserlib


#endif //PARSERLIB_RULE_HPP
