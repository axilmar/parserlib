#ifndef PARSERLIB_OPTIONALPARSER_HPP
#define PARSERLIB_OPTIONALPARSER_HPP


#include "Parser.hpp"


namespace parserlib {


    template <class Child>
    class OptionalParser : public Parser<OptionalParser<Child>> {
    public:
        OptionalParser(const Child& child)
            : m_child(child)
        {
        }

        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            m_child.parse(pc);
            return true;
        }

    private:
        Child m_child;
    };


} //namespace parserlib


#endif //PARSERLIB_OPTIONALPARSER_HPP
