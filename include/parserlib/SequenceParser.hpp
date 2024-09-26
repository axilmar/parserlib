#ifndef PARSERLIB_SEQUENCEPARSER_HPP
#define PARSERLIB_SEQUENCEPARSER_HPP


#include "Parser.hpp"
#include "LeftRecursion.hpp"


namespace parserlib {


    template <class Left, class Right>
    class SequenceParser : public Parser<SequenceParser<Left, Right>> {
    public:
        SequenceParser(const Left& left, const Right& right)
            : m_left(left)
            , m_right(right)
        {
        }

        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            auto state = pc.getParseState();

            try {
                if (!m_left.parse(pc)) {
                    pc.setParseState(state);
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
