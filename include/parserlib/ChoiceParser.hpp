#ifndef PARSERLIB_CHOICEPARSER_HPP
#define PARSERLIB_CHOICEPARSER_HPP


#include <type_traits>
#include "ParserNode.hpp"
#include "TerminalParser.hpp"
#include "TerminalStringParser.hpp"


namespace parserlib {


    /**
     * Choice of parsers.
     * At least one parser must parse successfully in order to parse the choice successfully.
     * @param Children children parser nodes.
     */
    template <class ...Children> class ChoiceParser : public ParserNode<ChoiceParser<Children...>> {
    public:
        /**
         * Constructor.
         * @param children children nodes.
         */
        ChoiceParser(const std::tuple<Children...>& children) 
            : m_children(children) {
        }

        /**
         * Returns the children nodes.
         * @return the children nodes.
         */
        const std::tuple<Children...>& children() const {
            return m_children;
        }

        /**
         * Invokes all child parsers, one by one, until one returns true.
         * @param pc parse context.
         * @return true if parsing succeeds, false otherwise.
         */
        template <class ParseContextType> bool operator ()(ParseContextType& pc) const {
            const auto errorState = pc.errorState();
            if (parse<0>(pc, [&](const auto& child) { return child(pc); })) {
                pc.setErrorState(errorState);
                return true;
            }
            return false;
        }

        /**
         * Invokes all child parsers, one by one, until one returns true.
         * The object is called to parse within a left recursion parsing context,
         * in order to continue parsing after the non-left recursive part is parsed.
         * @param pc parse context.
         * @param lrc left recursion context.
         * @return true if parsing succeeds, false otherwise.
         */
        template <class ParseContextType> bool parseLeftRecursionContinuation(ParseContextType& pc, LeftRecursionContext<ParseContextType>& lrc) const {
            const auto errorState = pc.errorState();
            if (parseLRC<0>(pc, lrc)) {
                pc.setErrorState(errorState);
                return true;
            }
            return false;
        }

    private:
        std::tuple<Children...> m_children;

        template <size_t Index, class ParseContextType, class PF> bool parse(ParseContextType& pc, const PF& pf) const {
            if constexpr (Index < sizeof...(Children)) {
                if (pf(std::get<Index>(m_children))) {
                    return true;
                }
                return parse<Index + 1>(pc, pf);
            }
            else {
                return false;
            }
        }

        template <size_t Index, class ParseContextType> bool parseLRC(ParseContextType& pc, LeftRecursionContext<ParseContextType>& lrc) const {
            if constexpr (Index < sizeof...(Children)) {
                lrc.setContinuationResolved(false);
                if (std::get<Index>(m_children).parseLeftRecursionContinuation(pc, lrc)) {
                    return true;
                }
                return parseLRC<Index + 1>(pc, lrc);
            }
            else {
                return false;
            }
        }
    };


    /**
     * Creates a choice of parsers out of two parsers.
     * @param node1 1st node.
     * @param node2 2nd node.
     * @return a choice of parsers.
     */
    template <class ParserNodeType1, class ParserNodeType2>
    ChoiceParser<ParserNodeType1, ParserNodeType2>
    operator | (const ParserNode<ParserNodeType1>& node1, const ParserNode<ParserNodeType2>& node2) {
        return ChoiceParser<ParserNodeType1, ParserNodeType2>(
            std::make_tuple(
                static_cast<const ParserNodeType1&>(node1), 
                static_cast<const ParserNodeType2&>(node2)));
    }


    /**
     * Creates a choice of parsers out of two choice parsers.
     * It flattens the structures into one choice.
     * @param node1 1st node.
     * @param node2 2nd node.
     * @return a choice of parsers.
     */
    template <class ...Children1, class ...Children2>
    ChoiceParser<Children1..., Children2...>
    operator | (const ParserNode<ChoiceParser<Children1...>>& node1, const ParserNode<ChoiceParser<Children2...>>& node2) {
        return ChoiceParser<Children1..., Children2...>(
            std::tuple_cat(
                static_cast<const ChoiceParser<Children1...>&>(node1).children(), 
                static_cast<const ChoiceParser<Children2...>&>(node2).children()));
    }


    /**
     * Creates a choice of parsers out of a choice parser and a non-choice parser.
     * @param node1 1st node.
     * @param node2 2nd node.
     * @return a choice of parsers.
     */
    template <class ...Children1, class ParserNodeType2>
    ChoiceParser<Children1..., ParserNodeType2>
    operator | (const ParserNode<ChoiceParser<Children1...>>& node1, const ParserNode<ParserNodeType2>& node2) {
        return ChoiceParser<Children1..., ParserNodeType2>(
            std::tuple_cat(
                static_cast<const ChoiceParser<Children1...>&>(node1).children(), 
                std::make_tuple(static_cast<const ParserNodeType2&>(node2))));
    }


    /**
     * Creates a choice of parsers out of a non-choice parser and a choice parser.
     * It flattens the structures into one choice.
     * @param node1 1st node.
     * @param node2 2nd node.
     * @return a choice of parsers.
     */
    template <class ParserNodeType1, class ...Children2>
    ChoiceParser<ParserNodeType1, Children2...>
        operator | (const ParserNode<ParserNodeType1>& node1, const ParserNode<ChoiceParser<Children2...>>& node2) {
        return ChoiceParser<ParserNodeType1, Children2...>(
            std::tuple_cat(
                std::make_tuple(static_cast<const ParserNodeType1&>(node1)), 
                static_cast<const ChoiceParser<Children2...>&>(node2).children()));
    }


    /**
     * Creates a choice of parsers out of a parser node and a terminal.
     * @param node parser node.
     * @param term terminal.
     * @return a sequence of parsers.
     */
    template <class ParserNodeType, class TerminalType, std::enable_if_t<!std::is_base_of_v<ParserNodeBase, TerminalType>, int> = 0>
    auto operator | (const ParserNode<ParserNodeType>& node, const TerminalType& term) {
        return node | terminal(term);
    }


    /**
     * Creates a choice of parsers out of a terminal and a parser node.
     * @param term terminal.
     * @param node parser node.
     * @return a sequence of parsers.
     */
    template <class ParserNodeType, class TerminalType, std::enable_if_t<!std::is_base_of_v<ParserNodeBase, TerminalType>, int> = 0>
    auto operator | (const TerminalType& term, const ParserNode<ParserNodeType>& node) {
        return terminal(term) | node;
    }


} //namespace parserlib


#endif //PARSERLIB_CHOICEPARSER_HPP
