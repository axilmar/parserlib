#ifndef PARSERLIB_BOOL_PARSE_NODE_HPP
#define PARSERLIB_BOOL_PARSE_NODE_HPP


#include <type_traits>
#include "parse_node.hpp"


namespace parserlib {


    /**
     * Uses a boolean value as the result of parsing.
     * Useful in forming logical expressions.
     */
    class bool_parse_node : public parse_node<bool_parse_node> {
    public:
        /**
         * The constructor.
         * @param value the value to return from the `parse()` function.
         */
        bool_parse_node(bool value) : m_value(value) {
        }

        /**
         * Returns the supplied boolean value.
         * The parse context is not modified.
         * @param pc the current parse context.
         * @return true or false.
         */
        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            return m_value;
        }

        #ifndef NDEBUG
        std::string text() const {
            return m_value ? "true" : "false";
        }
        #endif

    private:
        const bool m_value;
    };


    /**
     * Creates a parse node out of a bool value.
     * Provided as a template function with an enabled_if guard
     * in order to avoid accidental conversions to bool.
     * @param value the bool value to create a bool parse node from.
     * @return a bool parse node.
     */
    template <class T, std::enable_if_t<std::is_same_v<T, bool>, bool> = true>
    inline bool_parse_node make_parse_node(T value) {
        return bool_parse_node(value);
    }


} //namespace parserlib


#endif //PARSERLIB_BOOL_PARSE_NODE_HPP
