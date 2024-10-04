#ifndef PARSERLIB_CORE_ERRORRESUMEPARSER_HPP
#define PARSERLIB_CORE_ERRORRESUMEPARSER_HPP


#include "Parser.hpp"
#include "ParseError.hpp"


namespace parserlib::core {


    /**
     * An error resume point.
     * @param Parser type of parser to use for parsing an error resume point.
     */
    template <class Parser>
    class ErrorResumePoint {
    public:
        /**
         * The constructor.
         * @param parser parser to use for parsing an error resume point.
         */
        ErrorResumePoint(const Parser& parser)
            : m_parser(parser)
        {
        }

        /**
         * Returns the parser.
         * @return the parser.
         */
        const Parser& getParser() const {
            return m_parser;
        }

    private:
        Parser m_parser;
    };


    /**
     * An error resume parser.
     * 
     * It allows resumption of parsing after the error resume point.
     * 
     * @param Left the left part of the expression.
     * 
     * @param Right the right part of the expression, which is also used as an error resume point.
     */
    template <class Left, class Right>
    class ErrorResumeParser : public Parser<ErrorResumeParser<Left, Right>> {
    public:
        /**
         * The constructor.
         * @param left the left parser.
         * @param right the right parser.
         */
        ErrorResumeParser(const Left& left, const Right& right)
            : m_left(left)
            , m_right(right)
        {
        }

        /**
         * Tries to parse with the left parser, and if that suceeds,
         * then it tries to parse with the right parser.
         * 
         * In case of error, it iterates over the source,
         * using the right parser, in order to find the resume point.
         * 
         * If the resume point is found, then an error is added 
         * and errors in the parse context are committed 
         * (i.e. they are established and can't be replaced from further parsing).
         * Parsing then resumes after the resume point.
         * 
         * If no resume point is found, then the whole parse context is rewinded
         * to the starting point (the position of parsing before this is called), 
         * and false is returned.
         */
        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            auto beginParseState = pc.getParseState();

            if (m_left.parse(pc)) {
                if (m_right.parse(pc)) {
                    return true;
                }
            }

            while (!pc.isEndPosition()) {
                pc.incrementPosition();
                if (m_right.parse(pc)) {
                    pc.addError(ParseErrorType::SyntaxError, beginParseState.getPosition(), pc.getCurrentPosition());
                    return true;
                }
            }

            pc.setParseState(beginParseState);
            return false;
        }

    private:
        Left m_left;
        Right m_right;
    };


} //namespace parserlib::core


#endif //PARSERLIB_CORE_ERRORRESUMEPARSER_HPP
