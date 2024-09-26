#ifndef PARSERLIB_ANDPARSER_HPP
#define PARSERLIB_ANDPARSER_HPP


#include "Parser.hpp"


namespace parserlib {


    template <class Child>
    class AndParser : public Parser<AndParser<Child>> {
    public:
        AndParser(const Child& child)
            : m_child(child)
        {
        }

        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            auto state = pc.getState();
            const bool result = m_child.parse(pc);
            pc.setState(state);
            return result;
        }

    private:
        Child m_child;
    };


} //namespace parserlib


#endif //PARSERLIB_ANDPARSER_HPP
