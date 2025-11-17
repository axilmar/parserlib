#ifndef PARSERLIB_LOOP_BREAK_PARSE_NODE_HPP
#define PARSERLIB_LOOP_BREAK_PARSE_NODE_HPP


#include <sstream>
#include "parse_node.hpp"


namespace parserlib {


    /**
     * Exception thown to break the current loop.
     */
    struct loop_break_exception {
        /**
         * Number of loops to break.
         */
        const size_t levels;
    };


    /**
     * A parse node that can break a loop by throwing an exception.
     * The loop will take the exception and break out of the loop algorithm.
     */
    class loop_break_parse_node : public parse_node<loop_break_parse_node> {
    public:
        /**
         * The constructor.
         * @param levels levels of loops to break.
         */
        loop_break_parse_node(size_t levels) : m_levels(levels) {
        }

        /**
         * Throws a loop break exception with the supplied levels value.
         * @param pc the current parse context.
         * @exception loop_break_exception.
         */
        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            throw loop_break_exception{ m_levels };
        }

        #ifndef NDEBUG
        std::string text() const {
            std::stringstream stream;
            stream << "loop_break(" << m_levels << ')';
            return stream.str();
        }
        #endif

    private:
        const size_t m_levels;
    };


    /**
     * Creates a loop break parse node.
     * @param levels levels of loops to break.
     */
    inline loop_break_parse_node loop_break(size_t levels = 1) {
        return { levels };
    }


} //namespace parserlib


#endif PARSERLIB_LOOP_BREAK_PARSE_NODE_HPP
