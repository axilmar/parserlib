#ifndef PARSERLIB_TERMINALRANGEPARSER_HPP
#define PARSERLIB_TERMINALRANGEPARSER_HPP


#include <cassert>
#include "Parser.hpp"


namespace parserlib {


    /**
     * A parser that parses a terminal within a range of values.
     *
     * @param Char type of terminal value to parse.
     */
    template <class Char>
    class TerminalRangeParser : public Parser<TerminalRangeParser<Char>> {
    public:
        /**
         * The constructor.
         * @param min min value.
         * @param max max value.
         */
        TerminalRangeParser(const Char& min, const Char& max)
            : m_min(min)
            , m_max(max)
        {
            assert(min <= max);
        }

        /**
         * Parses a value within a range.
         * @param pc parse context.
         * @return true if parsing succeeds, false otherwise.
         */
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

            return false;
        }

    private:
        Char m_min;
        Char m_max;
    };


    /**
     * Creates a terminal parser.
     * @param min min value.
     * @param max max value.
     * @return a terminal range parser.
     */
    template <class Char>
    TerminalRangeParser<char> oneIn(const Char& min, const Char& max) {
        return TerminalRangeParser<char>(min, max);
    }


} //namespace parserlib


#endif //PARSERLIB_TERMINALRANGEPARSER_HPP
