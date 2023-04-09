#ifndef PARSERLIB_SEQUENCEPARSER_HPP
#define PARSERLIB_SEQUENCEPARSER_HPP


#include "ParserNode.hpp"
#include "LeftRecursionAccepted.hpp"


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
            //keep initial state in order to later restore it if subsequence parser fails
            const auto state = pc.state();

            //parse children; all children must parse
            const bool ok = parse<0>(pc);

            //if parsing fails, restore the initial state
            if (!ok) {
                pc.setState(state);
            }

            return ok;
        }

        /**
         * Parses a terminal under left recursion.
         * @param pc parse context.
         * @return true if a terminal was parsed, false otherwise.
         */
        template <class ParseContextType> bool parseLeftRecursionTerminal(ParseContextType& pc) const {
            //keep initial state in order to later restore it if subsequence parser fails
            const auto state = pc.state();

            //parse children; all children must parse
            const bool ok = parseLRT<0>(pc);

            //if parsing fails, restore the initial state
            if (!ok) {
                pc.setState(state);
            }

            return ok;
        }

        /**
         * Parses a left recursion continuation.
         * @param pc parse context.
         * @return true on success, false otherwise.
         */
        template <class ParseContextType> bool parseLeftRecursionContinuation(ParseContextType& pc) const {
            //keep initial state in order to later restore it if subsequence parser fails
            const auto state = pc.state();

            //parse children; all children must parse
            const bool ok = parseLRC<0>(pc);

            //if parsing fails, restore the initial state
            if (!ok) {
                pc.setState(state);
            }

            return ok;
        }

    private:
        std::tuple<Children...> m_children;

        template <size_t Index, class ParseContextType> bool parse(ParseContextType& pc) const {
            if constexpr (Index < sizeof...(Children)) {
                if (std::get<Index>(m_children)(pc)) {
                    return parse<Index + 1>(pc);
                }
                return false;
            }
            else {
                return true;
            }
        }

        template <size_t Index, class ParseContextType> bool parseLRT(ParseContextType& pc) const {
            if constexpr (Index < sizeof...(Children)) {
                const auto startPosition = pc.sourcePosition();
                
                //if parsing fails, return failure
                if (!std::get<Index>(m_children).parseLeftRecursionTerminal(pc)) {
                    return false;
                }

                //if no input was consumed, find a terminal from the next parser
                if (pc.sourcePosition() == startPosition) {
                    return parseLRT<Index + 1>(pc);
                }

                //input was consumed, so proceed with normal parsing
                return parse<Index + 1>(pc);
            }
            else {
                return true;
            }
        }

        template <size_t Index, class ParseContextType> bool parseLRC(ParseContextType& pc) const {
            if constexpr (Index < sizeof...(Children)) {
                try {
                    //if parsing fails, return failure
                    if (!std::get<Index>(m_children).parseLeftRecursionContinuation(pc)) {
                        return false;
                    }
                }

                //if a rule accepted the left recursion, then continue with normal parsing
                catch (LeftRecursionAccepted) {
                    return parse<Index + 1>(pc);
                }

                //no rule accepted the left recursion yet, continue with left recursion continuation
                return parseLRC<Index + 1>(pc);
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


} //namespace parserlib


#endif //PARSERLIB_SEQUENCEPARSER_HPP
