#ifndef PARSERLIB_TERMINALSETPARSER_HPP
#define PARSERLIB_TERMINALSETPARSER_HPP


#include <vector>
#include <algorithm>
#include "util.hpp"
#include "Parser.hpp"
#include "ParseErrorType.hpp"


namespace parserlib {


    template <class Char>
    class TerminalSetParser : public Parser<TerminalSetParser<Char>> {
    public:
        template <class String>
        TerminalSetParser(const String& str)
            : m_set(str.begin(), str.end())
        {
            init();
        }

        TerminalSetParser(const Char* str)
            : m_set(str, str + getStringLength(str))
        {
            init();
        }

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

            pc.setError(ParseErrorType::SyntaxError);
            return false;
        }

    private:
        std::vector<Char> m_set;

        void init() {
            std::sort(m_set.begin(), m_set.end());
        }
    };


    template <class Char>
    TerminalSetParser<Char> oneOf(const Char* chars) {
        return TerminalSetParser<Char>(chars);
    }


} //namespace parserlib


#endif //PARSERLIB_TERMINALSETPARSER_HPP
