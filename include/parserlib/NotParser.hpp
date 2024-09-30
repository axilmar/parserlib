#ifndef PARSERLIB_NOTPARSER_HPP
#define PARSERLIB_NOTPARSER_HPP


#include "Parser.hpp"


namespace parserlib {


    /**
     * Logical NOT parser.
     *
     * The unary operator ! is used to create a logical NOT parser out of another parser.
     *
     * @param Child child parser.
     */
    template <class Child>
    class NotParser : public Parser<NotParser<Child>> {
    public:
        /**
         * The constructor.
         * @param child the child parser.
         */
        NotParser(const Child& child)
            : m_child(child)
        {
        }

        /**
         * Parses the input.
         * It invokes the child parser in order to get the parsing result,
         * then restores the state of the parse context to the one before this call.
         * @param pc parse context.
         * @return the opposite of what the child parser returns.
         */
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
