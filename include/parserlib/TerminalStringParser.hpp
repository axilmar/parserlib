#ifndef PARSERLIB_TERMINALSTRINGPARSER_HPP
#define PARSERLIB_TERMINALSTRINGPARSER_HPP


#include <vector>
#include "util.hpp"
#include "Parser.hpp"


namespace parserlib {


    /**
     * Parses a string.
     * 
     * @param Char type of terminal value to parse.
     */
    template <class Char> 
    class TerminalStringParser : public Parser<TerminalStringParser<Char>> {
    public:
        /**
         * Constructor from string object.
         * @param str string to parse.
         */
        template <class String>
        TerminalStringParser(const String& str)
            : m_string(str.begin(), str.end())
        {
        }

        /**
         * Constructor from null-terminated string.
         * @param str string to parse.
         */
        TerminalStringParser(const Char* str)
            : m_string(str, str + getStringLength(str))
        {
        }

        /**
         * Parses a string.
         * In order for the current position of the parse context to advance,
         * the whole string must be successfully parsed.
         * @param pc parse context.
         * @return true if parsing succeeds, false otherwise.
         */
        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            auto itStr = m_string.begin();
            auto itParse = pc.getCurrentPosition();

            for (;;) {
                if (itStr == m_string.end()) {
                    pc.incrementPosition(m_string.size());
                    return true;
                }

                if (itParse == pc.getEndPosition()) {
                    break;
                }

                if (*itParse != *itStr) {
                    break;
                }

                ++itStr;
                ++itParse;
            }

            return false;
        }

    private:
        std::vector<Char> m_string;
    };


    template <class String>
    TerminalStringParser<typename String::value_type> term(const String& str) {
        return TerminalStringParser<typename String::value_type>(str);
    }


    template <class Char>
    TerminalStringParser<Char> term(const Char* str) {
        return TerminalStringParser<Char>(str);
    }


} //namespace parserlib


#endif //PARSERLIB_TERMINALSTRINGPARSER_HPP
