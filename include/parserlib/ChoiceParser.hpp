#ifndef PARSERLIB_CHOICEPARSER_HPP
#define PARSERLIB_CHOICEPARSER_HPP


#include "ParserNode.hpp"


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
            return parse<0>(pc);
        }

        /**
         * Parses a terminal under left recursion.
         * @param pc parse context.
         * @return true if a terminal was parsed, false otherwise.
         */
        template <class ParseContextType> bool parseLeftRecursionTerminal(ParseContextType& pc) const {
            return parseLRT<0>(pc);
        }

        /**
         * Parses a left recursion continuation.
         * @param pc parse context.
         * @return true on success, false otherwise.
         */
        template <class ParseContextType> bool parseLeftRecursionContinuation(ParseContextType& pc) const {
            return parseLRC<0>(pc);
        }

    private:
        std::tuple<Children...> m_children;

        template <size_t Index, class ParseContextType> bool parse(ParseContextType& pc) const {
            if constexpr (Index < sizeof...(Children)) {
                if (std::get<Index>(m_children)(pc)) {
                    return true;
                }
                return parse<Index + 1>(pc);
            }
            else {
                return false;
            }
        }

        template <size_t Index, class ParseContextType> bool parseLRT(ParseContextType& pc) const {
            if constexpr (Index < sizeof...(Children)) {
                if (std::get<Index>(m_children).parseLeftRecursionTerminal(pc)) {
                    return true;
                }
                return parseLRT<Index + 1>(pc);
            }
            else {
                return false;
            }
        }

        template <size_t Index, class ParseContextType> bool parseLRC(ParseContextType& pc) const {
            if constexpr (Index < sizeof...(Children)) {
                if (std::get<Index>(m_children).parseLeftRecursionContinuation(pc)) {
                    return true;
                }
                return parseLRC<Index + 1>(pc);
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


} //namespace parserlib


#endif //PARSERLIB_CHOICEPARSER_HPP
