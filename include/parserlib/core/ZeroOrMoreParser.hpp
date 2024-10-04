#ifndef PARSERLIB_CORE_ZEROORMOREPARSER_HPP
#define PARSERLIB_CORE_ZEROORMOREPARSER_HPP


#include "Parser.hpp"


namespace parserlib::core {


    /**
     * Loop 0 or more times.
     *
     * The unary operator * is used to create a zero-or-more parser out of another parser.
     *
     * @param Child child parser.
     */
    template <class Child>
    class ZeroOrMoreParser : public Parser<ZeroOrMoreParser<Child>> {
    public:
        /**
         * The constructor.
         * @param child the child parser.
         */
        ZeroOrMoreParser(const Child& child)
            : m_child(child)
        {
        }

        /**
         * Parses the input.
         * It invokes the child parser repeatedly until the child parser returns false.
         * @param pc parse context.
         * @return always true.
         */
        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            while (m_child.parse(pc)) {
            }
            return true;
        }

    private:
        Child m_child;
    };


} //namespace parserlib::core


#endif //PARSERLIB_CORE_ZEROORMOREPARSER_HPP
