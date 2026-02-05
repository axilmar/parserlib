#ifndef PARSERLIB_EXCEPTION_PARSE_NODE_HPP
#define PARSERLIB_EXCEPTION_PARSE_NODE_HPP


#include "parse_node.hpp"


namespace parserlib {


    /**
     * A parse node that throws an exception.
     * @param type of exception to throw.
     */
    template <class Exception>
    class exception_parse_node : public parse_node<exception_parse_node<Exception>> {
    public:
        /**
         * The constructor.
         * @param exception the object to throw as an exception.
         */
        exception_parse_node(const Exception& exception = Exception())
            : m_exception(exception)
        {
        }

        /**
         * Throws the specified exception.
         * @param pc the parse context.
         * @exception the specified exception.
         */
        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            throw m_exception;
        }

    private:
        Exception m_exception;
    };


    /**
     * Creates a exception parse node.
     * @param exception the exception to throw.
     * @return an exception parse node.
     */
    template <class Exception>
    exception_parse_node<Exception> exception(const Exception& exception) {
        return exception;
    }


    /**
     * Creates a exception parse node.
     * @param exception the exception to throw.
     * @return an exception parse node.
     */
    template <class Exception>
    exception_parse_node<Exception> exception() {
        return Exception();
    }


} //namespace parserlib


#endif //PARSERLIB_EXCEPTION_PARSE_NODE_HPP
