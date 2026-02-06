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
     * If parsing fails, any side effects, like errors or matches are canceled.
     * @param pc the parse context.
     * @param fn the parse function.
     * @return always true.
     */
    template <class ParseContext, class F>
    bool parse_optional(ParseContext& pc, const F& fn) {
        parse_and_restore_state_on_failure(pc, fn);
        return true;
    }


    /**
     * Invokes the given function repeatedly, until it fails to parse.
     * The loop also stops if no progress is made, i.e. when the parse context iterator
     * after the parse equals the parse context iterator before the parse.
     * If the loop stops, any side effects like errors or matches are canceled.
     * @param pc the parse context.
     * @param fn the parse function.
     * @return always true.
     */
    template <class ParseContext, class F>
    bool parse_loop_0(ParseContext& pc, const F& fn) {
        for(;;) {
            //store locally the required state
            const auto base_state = pc.get_state();

            //on success with progress, continue
            if (fn() && pc.get_iterator() != base_state.get_iterator()) {
                continue;
            }

            //failed to parse or suceeded but without progress; 
            //restore the state
            pc.set_state(base_state);
            break;
        }
        return true;
    }


} //namespace parserlib


#endif //PARSERLIB_PARSE_ALGORITHMS_HPP
