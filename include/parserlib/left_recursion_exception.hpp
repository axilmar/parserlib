#ifndef PARSERLIB_LEFT_RECURSION_EXCEPTION_HPP
#define PARSERLIB_LEFT_RECURSION_EXCEPTION_HPP


#include "parse_node_id_type.hpp"


namespace parserlib {


    /**
     * Exception thrown when there is a left recursion.
     */
    class left_recursion_exception {
    public:
        /**
         * The constructor.
         * @param parse_node_id id of the parse node that caused the left recursion.
         */
        left_recursion_exception(parse_node_id_type parse_node_id)
            : m_parse_node_id(parse_node_id)
        {
        }

        /**
         * Returns the id of the parse node that caused the left recursion.
         * @return the id of the parse node that caused the left recursion.
         */
        parse_node_id_type get_parse_node_id() const {
            return m_parse_node_id;
        }

    private:
        const parse_node_id_type m_parse_node_id;
    };


} //namespace parserlib


#endif //PARSERLIB_LEFT_RECURSION_EXCEPTION_HPP
