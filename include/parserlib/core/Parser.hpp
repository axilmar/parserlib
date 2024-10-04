#ifndef PARSERLIB_CORE_PARSER_HPP
#define PARSERLIB_CORE_PARSER_HPP


#include <string>
#include <type_traits>


namespace parserlib::core {


    template <class Child>
    class NotParser;


    template <class Child>
    class AndParser;


    template <class Child>
    class OptionalParser;


    template <class Child>
    class ZeroOrMoreParser;


    template <class Child>
    class OneOrMoreParser;


    template <class Left, class Right>
    class SequenceParser;


    template <class Left, class Right>
    class ChoiceParser;


    template <class Value>
    class TerminalParser;


    template <class Value>
    class TerminalStringParser;


    template <class MatchID, class Source>
    class Match;


    template <class Child, class MatchID>
    class MatchParser;


    template <class Parser>
    class ErrorResumePoint;


    template <class Left, class Right>
    class ErrorResumeParser;


    /**
     * Base class for parsers.
     * 
     * @param Derived type of class derived from this.
     */
    template <class Derived>
    class Parser {
    public:
        /**
         * Makes this parser a zero-or-more parser.
         * @return a zero-or-more parser that calls this parser as a child.
         */
        ZeroOrMoreParser<Derived> operator *() const {
            return ZeroOrMoreParser<Derived>(self());
        }

        /**
         * Makes this parser an one-or-more parser.
         * @return an one-or-more parser that calls this parser as a child.
         */
        OneOrMoreParser<Derived> operator +() const {
            return OneOrMoreParser<Derived>(self());
        }

        /**
         * Makes this parser optional.
         * @return an optional parser that calls this parser as a child.
         */
        OptionalParser<Derived> operator -() const {
            return OptionalParser<Derived>(self());
        }

        /**
         * Makes this parser be used as a logical NOT predicate.
         * @return a logical NOT parser that calls this parser as a child.
         */
        NotParser<Derived> operator !() const {
            return NotParser<Derived>(self());
        }

        /**
         * Makes this parser be used as a logical AND predicate.
         * @return a logical AND parser that calls this parser as a child.
         */
        AndParser<Derived> operator &() const {
            return AndParser<Derived>(self());
        }

        /**
         * Makes this parser an error resume point.
         * @return an error resume point.
         */
        ErrorResumePoint<Derived> operator ~() const {
            return ErrorResumePoint<Derived>(self());
        }

        /**
         * Casts this into Derived.
         * @return reference to derived.
         */
        const Derived& self() const {
            return *(const Derived*)this;
        }
    };


    /**
     * Creates a sequence of the given parsers.
     * @param left the left parser/element.
     * @param right the right parser/element.
     * @return a sequence parser for the given left and right parsers/elements.
     */
    template <class Left, class Right>
    SequenceParser<Left, Right> 
        operator >> (const Parser<Left>& left, const Parser<Right>& right)
    {
        return SequenceParser<Left, Right>(
            left.self(), right.self());
    }


    /**
     * Creates a sequence of the given parsers.
     * The right element is treated as a terminal.
     * @param left the left parser/element.
     * @param right the right parser/element.
     * @return a sequence parser for the given left and right parsers/elements.
     */
    template <class Left, class Right, std::enable_if_t<!std::is_base_of_v<Parser<Right>, Right>, bool> = true>
    SequenceParser<Left, TerminalParser<Right>> 
        operator >> (const Parser<Left>& left, const Right& right)
    {
        return SequenceParser<Left, TerminalParser<Right>>(
            left.self(), TerminalParser<Right>(right));
    }


    /**
     * Creates a sequence of the given parsers.
     * The right element is treated as a null-terminated terminal string.
     * @param left the left parser/element.
     * @param right the right parser/element.
     * @return a sequence parser for the given left and right parsers/elements.
     */
    template <class Left, class Right>
    SequenceParser<Left, TerminalStringParser<Right>> 
        operator >> (const Parser<Left>& left, const Right* right)
    {
        return SequenceParser<Left, TerminalStringParser<Right>>(
            left.self(), TerminalStringParser<Right>(right));
    }


    /**
     * Creates a sequence of the given parsers.
     * @param left the left parser/element.
     * @param right the right parser/element.
     * @return a sequence parser for the given left and right parsers/elements.
     */
    template <class Left, class CharT, class Traits = std::char_traits<CharT>, class Allocator>
    SequenceParser<Left, TerminalStringParser<CharT>> 
        operator >> (const Parser<Left>& left, const std::basic_string<CharT, Traits, Allocator>& right)
    {
        return SequenceParser<Left, TerminalStringParser<CharT>>(
            left.self(), TerminalStringParser<CharT>(right));
    }


    /**
     * Creates a sequence of the given parsers.
     * The left element is treated as a terminal.
     * @param left the left parser/element.
     * @param right the right parser/element.
     * @return a sequence parser for the given left and right parsers/elements.
     */
    template <class Left, class Right, std::enable_if_t<!std::is_base_of_v<Parser<Left>, Left>, bool> = true>
    SequenceParser<TerminalParser<Left>, Right> 
        operator >> (const Left& left, const Parser<Right>& right)
    {
        return SequenceParser<TerminalParser<Left>, Right>(
            TerminalParser<Left>(left), right.self());
    }


    /**
     * Creates a sequence of the given parsers.
     * The left element is treated as a null-terminated terminal string.
     * @param left the left parser/element.
     * @param right the right parser/element.
     * @return a sequence parser for the given left and right parsers/elements.
     */
    template <class Left, class Right>
    SequenceParser<TerminalStringParser<Left>, Right> 
        operator >> (const Left* left, const Parser<Right>& right)
    {
        return SequenceParser<TerminalStringParser<Left>, Right>(
            TerminalStringParser<Left>(left), right.self());
    }


    /**
     * Creates a sequence of the given parsers.
     * @param left the left parser/element.
     * @param right the right parser/element.
     * @return a sequence parser for the given left and right parsers/elements.
     */
    template <class Right, class CharT, class Traits = std::char_traits<CharT>, class Allocator>
    SequenceParser<TerminalStringParser<CharT>, Right> 
        operator >> (const std::basic_string<CharT, Traits, Allocator>& left, const Parser<Right>& right)
    {
        return SequenceParser<TerminalStringParser<CharT>, Right>(
            TerminalStringParser<CharT>(left), right.self());
    }


    /**
     * Creates an error resume parser.
     * @param left the left parser/element.
     * @param right the right parser/element.
     * @return an error resume parser.
     */
    template <class Left, class Right>
    ErrorResumeParser<Left, Right>
        operator >> (const Parser<Left>& left, const ErrorResumePoint<Right>& right)
    {
        return ErrorResumeParser<Left, Right>(
            left.self(), right.getParser());
    }


    /**
     * Creates an error resume parser.
     * @param left the left parser/element.
     * @param right the error resume point.
     * @return an error resume parser.
     */
    template <class Left, class Right, std::enable_if_t<!std::is_base_of_v<Parser<Left>, Left>, bool> = true>
    ErrorResumeParser<TerminalParser<Left>, Right>
        operator >> (const Left& left, const ErrorResumePoint<Right>& right)
    {
        return ErrorResumeParser<Left, Right>(
            TerminalParser<Left>(left), right.getParser());
    }


    /**
     * Creates an error resume parser.
     * @param left the left parser/element.
     * @param right the error resume point.
     * @return an error resume parser.
     */
    template <class Left, class Right>
    ErrorResumeParser<TerminalStringParser<Left>, Right>
        operator >> (const Left* left, const ErrorResumePoint<Right>& right)
    {
        return ErrorResumeParser<Left, Right>(
            TerminalStringParser<Left>(left), right.getParser());
    }


    /**
     * Creates an error resume parser.
     * @param left the left parser/element.
     * @param right the error resume point.
     * @return an error resume parser.
     */
    template <class Right, class CharT, class Traits = std::char_traits<CharT>, class Allocator>
    ErrorResumeParser<TerminalStringParser<CharT>, Right>
        operator >> (const std::basic_string<CharT, Traits, Allocator>& left, const ErrorResumePoint<Right>& right)
    {
        return ErrorResumeParser<TerminalStringParser<CharT>, Right>(
            TerminalStringParser<CharT>(left), right.getParser());
    }


    /**
     * Creates a choice of the given parsers.
     * @param left the left parser/element.
     * @param right the right parser/element.
     * @return a choice parser for the given left and right parsers/elements.
     */
    template <class Left, class Right>
    ChoiceParser<Left, Right> 
        operator | (const Parser<Left>& left, const Parser<Right>& right)
    {
        return ChoiceParser<Left, Right>(
            left.self(), right.self());
    }


    /**
     * Creates a choice of the given parsers.
     * The right element is treated as a terminal.
     * @param left the left parser/element.
     * @param right the right parser/element.
     * @return a choice parser for the given left and right parsers/elements.
     */
    template <class Left, class Right, std::enable_if_t<!std::is_base_of_v<Parser<Right>, Right>, bool> = true>
    ChoiceParser<Left, TerminalParser<Right>> 
        operator | (const Parser<Left>& left, const Right& right)
    {
        return ChoiceParser<Left, TerminalParser<Right>>(
            left.self(), TerminalParser<Right>(right));
    }


    /**
     * Creates a choice of the given parsers.
     * The right element is treated as a null-terminated terminal string.
     * @param left the left parser/element.
     * @param right the right parser/element.
     * @return a choice parser for the given left and right parsers/elements.
     */
    template <class Left, class Right>
    ChoiceParser<Left, TerminalStringParser<Right>> 
        operator | (const Parser<Left>& left, const Right* right)
    {
        return ChoiceParser<Left, TerminalStringParser<Right>>(
            left.self(), TerminalStringParser<Right>(right));
    }


    /**
     * Creates a choice of the given parsers.
     * @param left the left parser/element.
     * @param right the right parser/element.
     * @return a choice parser for the given left and right parsers/elements.
     */
    template <class Left, class CharT, class Traits = std::char_traits<CharT>, class Allocator>
    ChoiceParser<Left, TerminalStringParser<CharT>> 
        operator | (const Parser<Left>& left, const std::basic_string<CharT, Traits, Allocator>& right)
    {
        return ChoiceParser<Left, TerminalStringParser<CharT>>(
            left.self(), TerminalStringParser<CharT>(right));
    }


    /**
     * Creates a choice of the given parsers.
     * The left element is treated as a terminal.
     * @param left the left parser/element.
     * @param right the right parser/element.
     * @return a choice parser for the given left and right parsers/elements.
     */
    template <class Left, class Right, std::enable_if_t<!std::is_base_of_v<Parser<Left>, Left>, bool> = true>
    ChoiceParser<TerminalParser<Left>, Right> 
        operator | (const Left& left, const Parser<Right>& right)
    {
        return ChoiceParser<TerminalParser<Left>, Right>(
            TerminalParser<Left>(left), right.self());
    }


    /**
     * Creates a choice of the given parsers.
     * The left element is treated as a null-terminated terminal string.
     * @param left the left parser/element.
     * @param right the right parser/element.
     * @return a choice parser for the given left and right parsers/elements.
     */
    template <class Left, class Right>
    ChoiceParser<TerminalStringParser<Left>, Right> 
        operator | (const Left* left, const Parser<Right>& right)
    {
        return ChoiceParser<TerminalStringParser<Left>, Right>(
            TerminalStringParser<Left>(left), right.self());
    }


    /**
     * Creates a choice of the given parsers.
     * @param left the left parser/element.
     * @param right the right parser/element.
     * @return a choice parser for the given left and right parsers/elements.
     */
    template <class Right, class CharT, class Traits = std::char_traits<CharT>, class Allocator>
    ChoiceParser<TerminalStringParser<CharT>, Right> 
        operator | (const std::basic_string<CharT, Traits, Allocator>& left, const Parser<Right>& right)
    {
        return ChoiceParser<TerminalStringParser<CharT>, Right>(
            TerminalStringParser<CharT>(left), right.self());
    }


    /**
     * Creates a match parser for the given parser.
     * @param parser parser to create a match parser for.
     * @param matchId id of the match.
     * @return a match parser for the given parser.
     */
    template <class Child, class MatchID> 
    MatchParser<Child, MatchID> operator ->* (const Parser<Child>& parser, const MatchID& matchId) {
        return MatchParser<Child, MatchID>(parser.self(), matchId);
    }


    /**
     * Creates a match parser for the given parser.
     * @param parser parser to create a match parser for.
     * @param matchId id of the match.
     * @return a match parser for the given parser.
     */
    template <class Left, class MatchID, std::enable_if_t<!std::is_base_of_v<Parser<Left>, Left>, bool> = true>
    MatchParser<TerminalParser<Left>, MatchID> operator ->* (const Left& left, const MatchID& matchId) {
        return MatchParser<TerminalParser<Left>, MatchID>(TerminalParser<Left>(left), matchId);
    }


    /**
     * Creates a match parser for the given parser.
     * @param parser parser to create a match parser for.
     * @param matchId id of the match.
     * @return a match parser for the given parser.
     */
    template <class Left, class MatchID>
    MatchParser<TerminalStringParser<Left>, MatchID> operator ->* (const Left* left, const MatchID& matchId) {
        return MatchParser<TerminalStringParser<Left>, MatchID>(TerminalStringParser<Left>(left), matchId);
    }


    /**
     * Creates a match parser for the given parser.
     * @param parser parser to create a match parser for.
     * @param matchId id of the match.
     * @return a match parser for the given parser.
     */
    template <class MatchID, class CharT, class Traits = std::char_traits<CharT>, class Allocator>
    MatchParser<TerminalStringParser<CharT>, MatchID> operator ->* (const std::basic_string<CharT, Traits, Allocator>& left, const MatchID& matchId) {
        return MatchParser<TerminalStringParser<CharT>, MatchID>(TerminalStringParser<CharT>(left), matchId);
    }


    /**
     * Creates a sequence of the given parsers,
     * where the right parser is placed into a logical NOT parser,
     * and followed by the left parser.
     * @param left the left parser/element.
     * @param right the right parser/element.
     * @return a sequence parser for the given left and right parsers/elements.
     */
    template <class Left, class Right>
    auto operator - (const Parser<Left>& left, const Parser<Right>& right) {
        return !right.self() >> left.self();
    }


    /**
     * Creates a sequence of the given parsers,
     * where the right parser is placed into a logical NOT parser,
     * and followed by the left parser.
     * The right parser is treated as a terminal.
     * @param left the left parser/element.
     * @param right the right parser/element.
     * @return a sequence parser for the given left and right parsers/elements.
     */
    template <class Left, class Right, std::enable_if_t<!std::is_base_of_v<Parser<Right>, Right>, bool> = true>
    auto operator - (const Parser<Left>& left, const Right& right) {
        return !TerminalParser<Right>(right) >> left.self();
    }


    /**
     * Creates a sequence of the given parsers,
     * where the right parser is placed into a logical NOT parser,
     * and followed by the left parser.
     * The right parser is treated as a null-terminated terminal string.
     * @param left the left parser/element.
     * @param right the right parser/element.
     * @return a sequence parser for the given left and right parsers/elements.
     */
    template <class Left, class Right>
    auto operator - (const Parser<Left>& left, const Right* right) {
        return !TerminalStringParser<Right>(right) >> left.self();
    }


    /**
     * Creates a sequence of the given parsers,
     * where the right parser is placed into a logical NOT parser,
     * and followed by the left parser.
     * @param left the left parser/element.
     * @param right the right parser/element.
     * @return a sequence parser for the given left and right parsers/elements.
     */
    template <class Left, class CharT, class Traits = std::char_traits<CharT>, class Allocator>
    auto operator - (const Parser<Left>& left, const std::basic_string<CharT, Traits, Allocator>& right) {
        return !TerminalStringParser<CharT>(right) >> left.self();
    }


    /**
     * Creates a sequence of the given parsers,
     * where the right parser is placed into a logical NOT parser,
     * and followed by the left parser.
     * The left parse is treated as a terminal.
     * @param left the left parser/element.
     * @param right the right parser/element.
     * @return a sequence parser for the given left and right parsers/elements.
     */
    template <class Left, class Right, std::enable_if_t<!std::is_base_of_v<Parser<Left>, Left>, bool> = true>
    auto operator - (const Left& left, const Parser<Right>& right) {
        return !right.self() >> TerminalParser<Left>(left);
    }


    /**
     * Creates a sequence of the given parsers,
     * where the right parser is placed into a logical NOT parser,
     * and followed by the left parser.
     * The left parser is treated as a null-terminated terminal string.
     * @param left the left parser/element.
     * @param right the right parser/element.
     * @return a sequence parser for the given left and right parsers/elements.
     */
    template <class Left, class Right>
    auto operator - (const Left* left, const Parser<Right>& right) {
        return !right.self() >> TerminalStringParser<Left>(left);
    }


    /**
     * Creates a sequence of the given parsers,
     * where the right parser is placed into a logical NOT parser,
     * and followed by the left parser.
     * @param left the left parser/element.
     * @param right the right parser/element.
     * @return a sequence parser for the given left and right parsers/elements.
     */
    template <class Right, class CharT, class Traits = std::char_traits<CharT>, class Allocator>
    auto operator - (const std::basic_string<CharT, Traits, Allocator>& left, const Parser<Right>& right) {
        return !right.self() >> TerminalStringParser<CharT>(left);
    }


} //namespace parserlib::core


#endif //PARSERLIB_CORE_PARSER_HPP
