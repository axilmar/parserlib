#ifndef PARSERLIB_ONEORMOREPARSER_HPP
#define PARSERLIB_ONEORMOREPARSER_HPP


#include "Parser.hpp"


namespace parserlib {


    template <class Child>
    class OneOrMoreParser : public Parser<OneOrMoreParser<Child>> {
    public:
        OneOrMoreParser(const Child& child)
            : m_child(child)
        {
        }

        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            if (!m_child.parse(pc)) {
                return false;
            }
            while (m_child.parse(pc)) {
            }
            return true;
        }

    private:
        Child m_child;
    };


} //namespace parserlib


#endif //PARSERLIB_ONEORMOREPARSER_HPP
