#ifndef PARSERLIB_CORE_TERMINALSETPARSER_HPP
#define PARSERLIB_CORE_TERMINALSETPARSER_HPP


#include <vector>
#include <algorithm>
#include "parserlib/util.hpp"
#include "Parser.hpp"


namespace parserlib::core {


    /**
     * Parses a value out of a set of values.
     * 
     * @param Char type of terminal value to parse.
     */
    template <class Char>
    class TerminalSetParser : public Parser<TerminalSetParser<Char>> {
    public:
        /**
         * Constructor from string object.
         * @param str string that contains the set of characters.
         */
        template <class String>
        TerminalSetParser(const String& str)
            : m_set(str.begin(), str.end())
        {
            init();
        }

        /**
         * Constructor from null-terminated string.
         * @param str string that contains the set of characters.
         */
        TerminalSetParser(const Char* str)
            : m_set(str, str + getStringLength(str))
        {
            init();
        }

        /**
         * Parses a single value out of a set of values.
         * The function std::upper_bound is used to locate the appropriate value.
         * @param pc parse context.
         * @return true if parsing succeeds, false otherwise.
         */
        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            if (pc.isEndPosition()) {
                return false;
            }

            const auto c = *pc.getCurrentPosition();

            auto it = std::upper_bound(m_set.begin(), m_set.end(), c, Less());

            if (it != m_set.begin() && *std::prev(it) == c) {
                pc.incrementPosition();
                return true;
            }

            return false;
        }

    private:
        std::vector<Char> m_set;

        void init() {
            std::sort(m_set.begin(), m_set.end());
        }
    };


    /**
     * Creates a terminal set parser.
     * @param str string that contains the set of characters.
     * @return a terminal set parser.
     */
    template <class String>
    TerminalSetParser<typename String::value_type> oneOf(const String& str) {
        return TerminalSetParser<typename String::value_type>(str);
    }


    /**
     * Creates a terminal set parser.
     * @param str string that contains the set of characters.
     * @return a terminal set parser.
     */
    template <class Char>
    TerminalSetParser<Char> oneOf(const Char* chars) {
        return TerminalSetParser<Char>(chars);
    }


    /**
     * Creates a terminal set parser.
     * @param char1 first char.
     * @param chars rest of chars
     * @return a terminal set parser.
     */
    template <class Char, class... Chars>
    TerminalSetParser<Char> oneOf(const Char& char1, const Chars&... chars) {
        std::vector<Char> charSet;
        charSet.push_back(char1);
        (charSet.push_back(chars), ...);
        return TerminalSetParser<Char>(charSet);
    }


} //namespace parserlib::core


#endif //PARSERLIB_CORE_TERMINALSETPARSER_HPP
