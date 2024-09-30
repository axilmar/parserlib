#ifndef PARSERLIB_ONEORMOREPARSER_HPP
#define PARSERLIB_ONEORMOREPARSER_HPP


#include "Parser.hpp"


namespace parserlib {


    /**
     * Loop 1 or more times.
     *
     * The unary operator + is used to create an one-or-more parser out of another parser.
     *
     * @param Child child parser.
     */
    template <class Child>
    class OneOrMoreParser : public Parser<OneOrMoreParser<Child>> {
    public:
        /**
         * The constructor.
         * @param child the child parser.
         */
        OneOrMoreParser(const Child& child)
            : m_child(child)
        {
        }

        /**
         * Parses the input.
         * It invokes the child parser repeatedly until the child parser returns false.
         * @param pc parse context.
         * @return false if the first parse fails, true otherwise.
         */
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
