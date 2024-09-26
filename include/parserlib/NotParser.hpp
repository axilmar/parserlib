#ifndef PARSERLIB_NOTPARSER_HPP
#define PARSERLIB_NOTPARSER_HPP


#include "Parser.hpp"


namespace parserlib {


    template <class Child>
    class NotParser : public Parser<NotParser<Child>> {
    public:
        NotParser(const Child& child)
            : m_child(child)
        {
        }

        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            auto state = pc.getState();
            const bool result = !m_child.parse(pc);
            pc.setState(state);
            return result;
        }

    private:
        Child m_child;
    };


} //namespace parserlib


#endif //PARSERLIB_NOTPARSER_HPP
