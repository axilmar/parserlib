#ifndef PARSERLIB_OPTIONALPARSER_HPP
#define PARSERLIB_OPTIONALPARSER_HPP


#include "Parser.hpp"


namespace parserlib {


    /**
     * Makes a parser optional.
     *
     * The unary operator - is used to create an optional parser out of another parser.
     *
     * @param Child child parser.
     */
    template <class Child>
    class OptionalParser : public Parser<OptionalParser<Child>> {
    public:
        /**
         * The constructor.
         * @param child the child parser.
         */
        OptionalParser(const Child& child)
            : m_child(child)
        {
        }

        /**
         * Parses the input.
         * It invokes the child parser once.
         * @param pc parse context.
         * @return always true.
         */
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
