#ifndef PARSERLIB_BOOL_PARSE_NODE_HPP
#define PARSERLIB_BOOL_PARSE_NODE_HPP


#include <type_traits>
#include "parse_node.hpp"


namespace parserlib {


    /**
     * A parse node that returns a boolean value.
     */ 
    class bool_parse_node : public parse_node<bool_parse_node> {
    public:
        /**
         * The constructor.
         * @param value the boolean value.
         */
        bool_parse_node(bool value)
            : m_value(value)
        {
        }

        /**
         * Returns the boolean value.
         * @param pc the parse context to use.
         * @return the boolean value.
         */
        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            return m_value;
        }

    private:
        bool m_value;
    };


    /**
     * Allows the creation of a boolean parse node.
     * It is declared as a templated function in order to avoid accidental conversions.
     * @param value the boolean value to create a parse node for.
     * @return a boolean parse node.
     */
    template <class T, std::enable_if_t<std::is_same_v<T, bool>, bool> = true>
    bool_parse_node make_parse_node(T value) {
        return value;
    }


} //namespace parserlib


#endif //PARSERLIB_BOOL_PARSE_NODE_HPP
