#ifndef PARSERLIB_CORE_CHOICEPARSER_HPP
#define PARSERLIB_CORE_CHOICEPARSER_HPP


#include "Parser.hpp"
#include "LeftRecursion.hpp"


namespace parserlib::core {


    /**
     * Choice of parsers.
     * 
     * If the left parser fails, then the right parser is invoked.
     * 
     * @param Left left parser.
     * @param Right right parser.
     */
    template <class Left, class Right>
    class ChoiceParser : public Parser<ChoiceParser<Left, Right>> {
    public:
        /**
         * The constructor.
         * @param left the left parser.
         * @param right the right parser.
         */
        ChoiceParser(const Left& left, const Right& right)
            : m_left(left)
            , m_right(right)
        {
        }

        /**
         * Parses the input.
         * 
         * It invokes the left parser first, and if the left parser succeeds,
         * then it returns true.
         * 
         * Otherwise, it rewinds the parse state and tries the left parser.
         * 
         * Also, on left recursion reject coming from the left parser, it tries the right parser.
         * 
         * @return true if parsing succeeds, false otherwise.
         */
        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            auto state = pc.getParseState();

            try {
                if (m_left.parse(pc)) {
                    return true;
                }
            }
            catch (LeftRecursion::Reject) {
            }

            pc.setParseState(state);

            if (m_right.parse(pc)) {
                return true;
            }

            pc.setParseState(state);
            return false;
        }

    private:
        Left m_left;
        Right m_right;
    };


} //namespace parserlib::core


#endif //PARSERLIB_CORE_CHOICEPARSER_HPP
