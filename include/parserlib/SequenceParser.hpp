#ifndef PARSERLIB_SEQUENCEPARSER_HPP
#define PARSERLIB_SEQUENCEPARSER_HPP


#include <type_traits>
#include "ParserNode.hpp"
#include "TerminalParser.hpp"
#include "TerminalStringParser.hpp"
#include "NotParser.hpp"


namespace parserlib {


    /**
     * Sequence of parsers.
     * All parsers must parse successfully in order to parse the sequence successfully.
     * @param Children children parser nodes.
     */
    template <class ...Children> class SequenceParser : public ParserNode<SequenceParser<Children...>> {
    public:
        /**
         * Constructor.
         * @param children children nodes.
         */
        SequenceParser(const std::tuple<Children...>& children) 
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
         * Invokes all child parsers, one by one, until one returns false.
         * @param pc parse context.
         * @return true if parsing succeeds, false otherwise.
         */
        template <class ParseContextType> bool operator ()(ParseContextType& pc) const {
            return parse(pc, [&]() { return parseLoop<0>(pc, [&](const auto& child) { return child(pc); }); });
        }

        /**
         * Invokes all child parsers, one by one, until one returns false.
         * The object is called to parse within a left recursion parsing context,
         * in order to continue parsing after the non-left recursive part is parsed.
         * @param pc parse context.
         * @param lrc left recursion context.
         * @return true if parsing succeeds, false otherwise.
         */
        template <class ParseContextType> bool parseLeftRecursionContinuation(ParseContextType& pc, LeftRecursionContext<ParseContextType>& lrc) const {
            return parse(pc, [&]() { return parseLoopLRC<0>(pc, lrc); });
        }

    private:
        std::tuple<Children...> m_children;

        template <class ParseContextType, class PF> bool parse(ParseContextType& pc, const PF& pf) const {
            //keep initial state in order to later restore it if subsequence parser fails
            const auto state = pc.state();

            //parse
            if (pf()) {
                return true;
            }

            //parse failed; restore the parse context 
            pc.setState(state);
            return false;
        }

        template <size_t Index, class ParseContextType, class PF> bool parseLoop(ParseContextType& pc, const PF& pf) const {
            if constexpr (Index < sizeof...(Children)) {
                if (pf(std::get<Index>(m_children))) {
                    return parseLoop<Index + 1>(pc, pf);
                }
                return false;
            }
            else {
                return true;
            }
        }

        template <size_t Index, class ParseContextType> bool parseLoopLRC(ParseContextType& pc, LeftRecursionContext<ParseContextType>& lrc) const {
            if constexpr (Index < sizeof...(Children)) {
                const auto startPosition = pc.sourcePosition();

                //parse child; if it fails, return false
                if (!std::get<Index>(m_children).parseLeftRecursionContinuation(pc, lrc)) {
                    return false;
                }

                //child parsed sucessfully
                
                //if the left recursion continuation is started, i.e. the current left recursive rule
                //has been called to parse the continuation, return to normal parsing
                if (lrc.continuationResolved()) {
                    return parseLoop<Index + 1>(pc, [&](const auto& child) { return child(pc); });
                }

                //try the next child for continuation
                return parseLoopLRC<Index + 1>(pc, lrc);
            }
            else {
                return true;
            }
        }
    };


    /**
     * Creates a sequence of parsers out of two parsers.
     * @param node1 1st node.
     * @param node2 2nd node.
     * @return a sequence of parsers.
     */
    template <class ParserNodeType1, class ParserNodeType2>
    SequenceParser<ParserNodeType1, ParserNodeType2>
    operator >> (const ParserNode<ParserNodeType1>& node1, const ParserNode<ParserNodeType2>& node2) {
        return SequenceParser<ParserNodeType1, ParserNodeType2>(
            std::make_tuple(
                static_cast<const ParserNodeType1&>(node1), 
                static_cast<const ParserNodeType2&>(node2)));
    }


    /**
     * Creates a sequence of parsers out of two sequence parsers.
     * It flattens the structures into one sequence.
     * @param node1 1st node.
     * @param node2 2nd node.
     * @return a sequence of parsers.
     */
    template <class ...Children1, class ...Children2>
    SequenceParser<Children1..., Children2...>
    operator >> (const ParserNode<SequenceParser<Children1...>>& node1, const ParserNode<SequenceParser<Children2...>>& node2) {
        return SequenceParser<Children1..., Children2...>(
            std::tuple_cat(
                static_cast<const SequenceParser<Children1...>&>(node1).children(), 
                static_cast<const SequenceParser<Children2...>&>(node2).children()));
    }


    /**
     * Creates a sequence of parsers out of a sequence parser and a non-sequence parser.
     * @param node1 1st node.
     * @param node2 2nd node.
     * @return a sequence of parsers.
     */
    template <class ...Children1, class ParserNodeType2>
    SequenceParser<Children1..., ParserNodeType2>
    operator >> (const ParserNode<SequenceParser<Children1...>>& node1, const ParserNode<ParserNodeType2>& node2) {
        return SequenceParser<Children1..., ParserNodeType2>(
            std::tuple_cat(
                static_cast<const SequenceParser<Children1...>&>(node1).children(), 
                std::make_tuple(static_cast<const ParserNodeType2&>(node2))));
    }


    /**
     * Creates a sequence of parsers out of a non-sequence parser and a sequence parser.
     * It flattens the structures into one sequence.
     * @param node1 1st node.
     * @param node2 2nd node.
     * @return a sequence of parsers.
     */
    template <class ParserNodeType1, class ...Children2>
    SequenceParser<ParserNodeType1, Children2...>
        operator >> (const ParserNode<ParserNodeType1>& node1, const ParserNode<SequenceParser<Children2...>>& node2) {
        return SequenceParser<ParserNodeType1, Children2...>(
            std::tuple_cat(
                std::make_tuple(static_cast<const ParserNodeType1&>(node1)), 
                static_cast<const SequenceParser<Children2...>&>(node2).children()));
    }


    /**
     * Creates a sequence of parsers out of a parser node and a terminal.
     * @param node parser node.
     * @param term terminal.
     * @return a sequence of parsers.
     */
    template <class ParserNodeType, class TerminalType, std::enable_if_t<!std::is_base_of_v<ParserNodeBase, TerminalType>, int> = 0>
    auto operator >> (const ParserNode<ParserNodeType>& node, const TerminalType& term) {
        return node >> terminal(term);
    }


    /**
     * Creates a sequence of parsers out of a terminal and a parser node.
     * @param term terminal.
     * @param node parser node.
     * @return a sequence of parsers.
     */
    template <class ParserNodeType, class TerminalType, std::enable_if_t<!std::is_base_of_v<ParserNodeBase, TerminalType>, int> = 0>
    auto operator >> (const TerminalType& term, const ParserNode<ParserNodeType>& node) {
        return terminal(term) >> node;
    }


    /**
     * Creates a sequence of parsers out of two parsers, where the first parser is negated.
     * @param node1 1st node.
     * @param node2 2nd node.
     * @return a sequence of parsers.
     */
    template <class ParserNodeType1, class ParserNodeType2>
    auto operator - (const ParserNode<ParserNodeType1>& node1, const ParserNode<ParserNodeType2>& node2) {
        return !node2 >> node1;
    }


    /**
     * Creates a sequence of parsers out of a parser node and a terminal, where the terminal is negated.
     * @param node parser node.
     * @param term terminal.
     * @return a sequence of parsers.
     */
    template <class ParserNodeType, class TerminalType, std::enable_if_t<!std::is_base_of_v<ParserNodeBase, TerminalType>, int> = 0>
    auto operator - (const ParserNode<ParserNodeType>& node, const TerminalType& term) {
        return node - terminal(term);
    }


    /**
     * Creates a sequence of parsers out of a terminal and a parser node, where the parser node is negated.
     * @param term terminal.
     * @param node parser node.
     * @return a sequence of parsers.
     */
    template <class ParserNodeType, class TerminalType, std::enable_if_t<!std::is_base_of_v<ParserNodeBase, TerminalType>, int> = 0>
    auto operator - (const TerminalType& term, const ParserNode<ParserNodeType>& node) {
        return terminal(term) - node;
    }


} //namespace parserlib


#endif //PARSERLIB_SEQUENCEPARSER_HPP
