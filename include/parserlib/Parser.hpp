#ifndef PARSERLIB_PARSER_HPP
#define PARSERLIB_PARSER_HPP


#include <string>
#include <type_traits>


namespace parserlib {


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


    template <class MatchId, class Source>
    class Match;


    template <class Child, class MatchId>
    class MatchParser;


    template <class Derived>
    class Parser {
    public:
        ZeroOrMoreParser<Derived> operator *() const {
            return ZeroOrMoreParser<Derived>(self());
        }

        OneOrMoreParser<Derived> operator +() const {
            return OneOrMoreParser<Derived>(self());
        }

        OptionalParser<Derived> operator -() const {
            return OptionalParser<Derived>(self());
        }

        NotParser<Derived> operator !() const {
            return NotParser<Derived>(self());
        }

        AndParser<Derived> operator &() const {
            return AndParser<Derived>(self());
        }

        const Derived& self() const {
            return *(const Derived*)this;
        }
    };


    template <class Left, class Right>
    SequenceParser<Left, Right> 
        operator >> (const Parser<Left>& left, const Parser<Right>& right)
    {
        return SequenceParser<Left, Right>(
            left.self(), right.self());
    }


    template <class Left, class Right, std::enable_if_t<!std::is_base_of_v<Parser<Right>, Right>, bool> = true>
    SequenceParser<Left, TerminalParser<Right>> 
        operator >> (const Parser<Left>& left, const Right& right)
    {
        return SequenceParser<Left, TerminalParser<Right>>(
            left.self(), TerminalParser<Right>(right));
    }


    template <class Left, class Right>
    SequenceParser<Left, TerminalStringParser<Right>> 
        operator >> (const Parser<Left>& left, const Right* right)
    {
        return SequenceParser<Left, TerminalStringParser<Right>>(
            left.self(), TerminalStringParser<Right>(right));
    }


    template <class Left, class CharT, class Traits = std::char_traits<CharT>, class Allocator>
    SequenceParser<Left, TerminalStringParser<CharT>> 
        operator >> (const Parser<Left>& left, const std::basic_string<CharT, Traits, Allocator>& right)
    {
        return SequenceParser<Left, TerminalStringParser<CharT>>(
            left.self(), TerminalStringParser<CharT>(right));
    }


    template <class Left, class Right, std::enable_if_t<!std::is_base_of_v<Parser<Left>, Left>, bool> = true>
    SequenceParser<TerminalParser<Left>, Right> 
        operator >> (const Left& left, const Parser<Right>& right)
    {
        return SequenceParser<TerminalParser<Left>, Right>(
            TerminalParser<Left>(left), right.self());
    }


    template <class Left, class Right>
    SequenceParser<TerminalStringParser<Left>, Right> 
        operator >> (const Left* left, const Parser<Right>& right)
    {
        return SequenceParser<TerminalStringParser<Left>, Right>(
            TerminalStringParser<Left>(left), right.self());
    }


    template <class Right, class CharT, class Traits = std::char_traits<CharT>, class Allocator>
    SequenceParser<TerminalStringParser<CharT>, Right> 
        operator >> (const std::basic_string<CharT, Traits, Allocator>& left, const Parser<Right>& right)
    {
        return SequenceParser<TerminalStringParser<CharT>, Right>(
            TerminalStringParser<CharT>(left), right.self());
    }


    template <class Left, class Right>
    ChoiceParser<Left, Right> 
        operator | (const Parser<Left>& left, const Parser<Right>& right)
    {
        return ChoiceParser<Left, Right>(
            left.self(), right.self());
    }


    template <class Left, class Right, std::enable_if_t<!std::is_base_of_v<Parser<Right>, Right>, bool> = true>
    ChoiceParser<Left, TerminalParser<Right>> 
        operator | (const Parser<Left>& left, const Right& right)
    {
        return ChoiceParser<Left, TerminalParser<Right>>(
            left.self(), TerminalParser<Right>(right));
    }


    template <class Left, class Right>
    ChoiceParser<Left, TerminalStringParser<Right>> 
        operator | (const Parser<Left>& left, const Right* right)
    {
        return ChoiceParser<Left, TerminalStringParser<Right>>(
            left.self(), TerminalStringParser<Right>(right));
    }


    template <class Left, class CharT, class Traits = std::char_traits<CharT>, class Allocator>
    ChoiceParser<Left, TerminalStringParser<CharT>> 
        operator | (const Parser<Left>& left, const std::basic_string<CharT, Traits, Allocator>& right)
    {
        return ChoiceParser<Left, TerminalStringParser<CharT>>(
            left.self(), TerminalStringParser<CharT>(right));
    }


    template <class Left, class Right, std::enable_if_t<!std::is_base_of_v<Parser<Left>, Left>, bool> = true>
    ChoiceParser<TerminalParser<Left>, Right> 
        operator | (const Left& left, const Parser<Right>& right)
    {
        return ChoiceParser<TerminalParser<Left>, Right>(
            TerminalParser<Left>(left), right.self());
    }


    template <class Left, class Right>
    ChoiceParser<TerminalStringParser<Left>, Right> 
        operator | (const Left* left, const Parser<Right>& right)
    {
        return ChoiceParser<TerminalStringParser<Left>, Right>(
            TerminalStringParser<Left>(left), right.self());
    }


    template <class Right, class CharT, class Traits = std::char_traits<CharT>, class Allocator>
    ChoiceParser<TerminalStringParser<CharT>, Right> 
        operator | (const std::basic_string<CharT, Traits, Allocator>& left, const Parser<Right>& right)
    {
        return ChoiceParser<TerminalStringParser<CharT>, Right>(
            TerminalStringParser<CharT>(left), right.self());
    }


    template <class Child, class MatchId> 
    MatchParser<Child, MatchId> operator ->* (const Parser<Child>& parser, const MatchId& matchId) {
        return MatchParser<Child, MatchId>(parser.self(), matchId);
    }

    template <class Left, class Right>
    auto operator - (const Parser<Left>& left, const Parser<Right>& right) {
        return !right.self() >> left.self();
    }


    template <class Left, class Right, std::enable_if_t<!std::is_base_of_v<Parser<Right>, Right>, bool> = true>
    auto operator - (const Parser<Left>& left, const Right& right) {
        return !TerminalParser<Right>(right) >> left.self();
    }


    template <class Left, class Right>
    auto operator - (const Parser<Left>& left, const Right* right) {
        return !TerminalStringParser<Right>(right) >> left.self();
    }


    template <class Left, class CharT, class Traits = std::char_traits<CharT>, class Allocator>
    auto operator - (const Parser<Left>& left, const std::basic_string<CharT, Traits, Allocator>& right) {
        return !TerminalStringParser<CharT>(right) >> left.self();
    }


    template <class Left, class Right, std::enable_if_t<!std::is_base_of_v<Parser<Left>, Left>, bool> = true>
    auto operator - (const Left& left, const Parser<Right>& right) {
        return !right.self() >> TerminalParser<Left>(left);
    }


    template <class Left, class Right>
    auto operator - (const Left* left, const Parser<Right>& right) {
        return !right.self() >> TerminalStringParser<Left>(left);
    }


    template <class Right, class CharT, class Traits = std::char_traits<CharT>, class Allocator>
    auto operator - (const std::basic_string<CharT, Traits, Allocator>& left, const Parser<Right>& right) {
        return !right.self() >> TerminalStringParser<CharT>(left);
    }


} //namespace parserlib


#endif //PARSERLIB_PARSER_HPP
