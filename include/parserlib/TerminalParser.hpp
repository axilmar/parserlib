#ifndef PARSERLIB_TERMINALPARSER_HPP
#define PARSERLIB_TERMINALPARSER_HPP


#include "Parser.hpp"
#include "ParseErrorType.hpp"


namespace parserlib {


    template <class Char>
    class TerminalParser : public Parser<TerminalParser<Char>> {
    public:
        typedef Char Char;

        TerminalParser(const Char& value)
            : m_char(value)
        {
        }

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


    template <class Char>
    TerminalParser<Char> term(const Char& value) {
        return TerminalParser<Char>(value);
    }


} //namespace parserlib


#endif //PARSERLIB_TERMINALPARSER_HPP
