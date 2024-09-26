#ifndef PARSERLIB_TERMINALRANGEPARSER_HPP
#define PARSERLIB_TERMINALRANGEPARSER_HPP


#include <cassert>
#include "Parser.hpp"
#include "ParseErrorType.hpp"


namespace parserlib {


    template <class Char>
    class TerminalRangeParser : public Parser<TerminalRangeParser<Char>> {
    public:
        TerminalRangeParser(const Char& min, const Char& max)
            : m_min(min)
            , m_max(max)
        {
            assert(min <= max);
        }

        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            if (pc.isEndPosition()) {
                return false;
            }

            const auto c = *pc.getCurrentPosition();

            if (c >= m_min && c <= m_max) {
                pc.incrementPosition();
                return true;
            }

            pc.setError(ParseErrorType::SyntaxError);
            return false;
        }

    private:
        Char m_min;
        Char m_max;
    };


    template <class Char>
    TerminalRangeParser<char> oneIn(const Char& min, const Char& max) {
        return TerminalRangeParser<char>(min, max);
    }


} //namespace parserlib


#endif //PARSERLIB_TERMINALRANGEPARSER_HPP
