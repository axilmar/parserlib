#ifndef PARSERLIB_ERROR_PARSE_NODE_HPP
#define PARSERLIB_ERROR_PARSE_NODE_HPP


#include "bool_parse_node.hpp"


namespace parserlib {


    /**
     * A parse node that adds a error to a parse context
     * when another node parses successfully.
     * @param ErrorId type of id of the parse node.
     * @param Child child type.
     */
    template <class ErrorId, class Child>
    class error_parse_node : public parse_node<error_parse_node<ErrorId, Child>> {
    public:
        /**
         * The constructor.
         * @param id the error id.
         * @param child the child.
         */
        error_parse_node(const ErrorId& id, const Child& child)
            : m_id(id)
            , m_child(child)
        {
        }

        /**
         * If the child parses successfully,
         * then it adds an error to the parse context.
         * @param pc the parse context.
         * @return true on success, false on failure.
         */
        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            const auto begin = pc.get_iterator();
            if (m_child.parse(pc)) {
                pc.add_error(m_id, begin, pc.get_iterator());
                return true;
            }
            return false;
        }

    private:
        ErrorId m_id;
        Child m_child;
    };


    /**
     * Creates a error parse node that calls another parse node to skip input.
     * @param id error id.
     * @param skip_val value/parse node to use for skipping input.
     * @return a error parse node.
     */
    template <class ErrorId, class T>
    auto error(const ErrorId& id, const T& skip_val) {
        return error_parse_node(id, make_parse_node(skip_val));
    }


    /**
     * Creates a error parse node that does not skip any input.
     * @param id error id.
     * @return a error parse node.
     */
    template <class ErrorId>
    auto error(const ErrorId& id) {
        return error_parse_node(id, true_);
    }


} //namespace parserlib


#endif //PARSERLIB_ERROR_PARSE_NODE_HPP
