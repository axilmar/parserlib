#ifndef PARSERLIB_SEQUENCEPARSER_HPP
#define PARSERLIB_SEQUENCEPARSER_HPP


#include "Parser.hpp"
#include "LeftRecursion.hpp"


namespace parserlib {


    /**
     * sequence of parsers.
     *
     * If the left parser succeeds, then the right parser is invoked.
     *
     * @param Left left parser.
     * @param Right right parser.
     */
    template <class Left, class Right>
    class SequenceParser : public Parser<SequenceParser<Left, Right>> {
    public:
        /**
         * The constructor.
         * @param left the left parser.
         * @param right the right parser.
         */
        SequenceParser(const Left& left, const Right& right)
            : m_left(left)
            , m_right(right)
        {
        }

        /**
         * Parses the input.
         *
         * It invokes the left parser first, and if the left parser fails,
         * then it returns false.
         *
         * Then it tries the right parser.
         * 
         * On left recursion accept coming from the left parser, it invokes the right parser.
         * 
         * If the right parser fails, then the parse context is restored to the state
         * that it was before invoking the left parser.
         *
         * @return true if parsing succeeds, false otherwise.
         */
        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            auto state = pc.getParseState();

            try {
                if (!m_left.parse(pc)) {
                    return false;
                }
            }
            catch (LeftRecursion::Accept) {
            }

            if (!m_right.parse(pc)) {
                pc.setParseState(state);
                return false;
            }

            return true;
        }

    private:
        Left m_left;
        Right m_right;
    };


} //namespace parserlib


#endif //PARSERLIB_SEQUENCEPARSER_HPP
