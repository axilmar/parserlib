#ifndef PARSERLIB_ZEROORMOREPARSER_HPP
#define PARSERLIB_ZEROORMOREPARSER_HPP


#include "Parser.hpp"


namespace parserlib {


    template <class Child>
    class ZeroOrMoreParser : public Parser<ZeroOrMoreParser<Child>> {
    public:
        ZeroOrMoreParser(const Child& child)
            : m_child(child)
        {
        }

        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            while (m_child.parse(pc)) {
            }
            return true;
        }

    private:
        Child m_child;
    };


} //namespace parserlib


#endif //PARSERLIB_ZEROORMOREPARSER_HPP
