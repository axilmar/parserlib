#ifndef PARSERLIB_PARSE_ALGORITHMS_HPP
#define PARSERLIB_PARSE_ALGORITHMS_HPP


namespace parserlib {


    /**
     * Executes a parse function.
     * If the function fails, then the state of the parse context is restored.
     * @param pc the parse context.
     * @param fn the parse function.
     * @return true if the function suceeds, false otherwise.
     */
    template <class ParseContext, class F>
    bool parse_and_restore_state_on_failure(ParseContext& pc, const F& fn) {
        const auto initial_state = pc.get_state();
        try {
            if (fn()) {
                return true;
            }
            pc.set_state(initial_state);
        }
        catch (...) {
            pc.set_state(initial_state);
            throw;
        }
        return false;
    }


    /**
     * Executes a parse function, then restores the state of the parse context.
     * @param pc the parse context.
     * @param fn the parse function.
     * @return true if the function suceeds, false otherwise.
     */
    template <class ParseContext, class F>
    bool parse_and_restore_state(ParseContext& pc, const F& fn) {
        const auto initial_state = pc.get_state();
        try {
            const bool result = fn();
            pc.set_state(initial_state);
            return result;
        }
        catch (...) {
            pc.set_state(initial_state);
            throw;
        }
    }


    /**
     * Invokes the given function.
     * @param pc the parse context.
     * @param fn the parse function.
     * @return always true.
     */
    template <class ParseContext, class F>
    bool parse_optional(ParseContext& pc, const F& fn) {
        fn();
        return true;
    }


    /**
     * Invokes the given function repeatedly, until it fails to parse.
     * The loop also stops if no progress is made.
     * @param pc the parse context.
     * @param fn the parse function.
     * @return always true.
     */
    template <class ParseContext, class F>
    bool parse_loop_0(ParseContext& pc, const F& fn) {
        for(;;) {
            const auto base_iterator = pc.get_iterator();
            if (!fn() || pc.get_iterator() == base_iterator) {
                break;
            }
        }
        return true;
    }


} //namespace parserlib


#endif //PARSERLIB_PARSE_ALGORITHMS_HPP
