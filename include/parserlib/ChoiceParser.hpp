#ifndef PARSERLIB_CHOICEPARSER_HPP
#define PARSERLIB_CHOICEPARSER_HPP


#include "Parser.hpp"
#include "LeftRecursion.hpp"


namespace parserlib {


    template <class Left, class Right>
    class ChoiceParser : public Parser<ChoiceParser<Left, Right>> {
    public:
        ChoiceParser(const Left& left, const Right& right)
            : m_left(left)
            , m_right(right)
        {
        }

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


} //namespace parserlib


#endif //PARSERLIB_CHOICEPARSER_HPP
