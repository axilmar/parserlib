#ifndef PARSERLIB_ANDPARSER_HPP
#define PARSERLIB_ANDPARSER_HPP


#include "Parser.hpp"


namespace parserlib {


    /**
     * Logical AND parser.
     * 
     * The unary operator & is used to create a logical AND parser out of another parser.
     * 
     * @param Child child parser.
     */
    template <class Child>
    class AndParser : public Parser<AndParser<Child>> {
    public:
        /**
         * The constructor.
         * @param child the child parser.
         */
        AndParser(const Child& child)
            : m_child(child)
        {
        }

        /**
         * Parses the input.
         * It invokes the child parser in order to get the parsing result,
         * then restores the state of the parse context to the one before this call.
         * @param pc parse context.
         * @return whatever the child parser returns.
         */
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
