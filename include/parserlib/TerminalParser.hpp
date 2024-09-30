#ifndef PARSERLIB_TERMINALPARSER_HPP
#define PARSERLIB_TERMINALPARSER_HPP


#include "Parser.hpp"
#include "ParseErrorType.hpp"


namespace parserlib {


    /**
     * A parser that parses a terminal.
     * 
     * @param Char type of terminal value to parse.
     */
    template <class Char>
    class TerminalParser : public Parser<TerminalParser<Char>> {
    public:
        /**
         * Type of terminal value to parse.
         */
        typedef Char Char;

        /**
         * Constructor from value.
         * @param value terminal value to parse.
         */
        TerminalParser(const Char& value)
            : m_char(value)
        {
        }

        /**
         * Parses the terminal.
         * @param pc parse context.
         * @return true if parsing succeeds, false otherwise.
         */
        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            if (pc.isEndPosition()) {
                return false;
            }

            if (m_char == *pc.getCurrentPosition()) {
                pc.incrementPosition();
                return true;
            }

            pc.setError(ParseErrorType::SyntaxError);
            return false;
        }

    private:
        Char m_char;
    };


    /**
     * Creates a terminal parser out of a value.
     * @param value terminal value to parse.
     * @return a terminal parser.
     */
    template <class Char>
    TerminalParser<Char> term(const Char& value) {
        return TerminalParser<Char>(value);
    }


} //namespace parserlib


#endif //PARSERLIB_TERMINALPARSER_HPP
