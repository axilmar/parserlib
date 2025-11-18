#ifndef PARSERLIB_LOOP_FUNCTIONS_HPP
#define PARSERLIB_LOOP_FUNCTIONS_HPP


#include "loop_break_parse_node.hpp"


namespace parserlib {


    /**
     * Helper function for loop implementations.
     * It executes the given function, and if a loop_break_exception is caught,
     * then it either returns the result_on_break value, if the break levels is 1,
     * or decreases the break levels and throws another loop_break_exception.
     * @param f the function to execute.
     * @param result_on_break what to return on loop break.
     * @return false/true depending on function and result_on_break parameter.
     */
    template <class F>
    bool do_catch_loop_break(const F& f, const bool result_on_break = true) {
        try {
            return f();
        }
        catch (loop_break_exception ex) {
            if (ex.levels > 1) {
                throw loop_break_exception{ ex.levels - 1 };
            }
        }
        return result_on_break;
    }


    /**
     * Helper function for executing a parser within a loop.
     * It handles loop breaks.
     * @param pc the current parse context.
     * @return always true.
     */
    template <class ParseContext, class ParseNode>
    bool loop_parse(ParseContext& pc, const ParseNode& parse_node) {
        return do_catch_loop_break([&]() {
            while (pc.parse_valid()) {
                const auto start_iterator = pc.parse_position().iterator();
                if (!pc.parse(parse_node) || pc.parse_position().iterator() == start_iterator) {
                    break;
                }
            }
            return true;
        });
    }


} //namespace parserlib


#endif //PARSERLIB_LOOP_FUNCTIONS_HPP
