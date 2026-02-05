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
     * If new errors are detected, they are canceled.
     * @param pc the parse context.
     * @param fn the parse function.
     * @return always true.
     */
    template <class ParseContext, class F>
    bool parse_optional(ParseContext& pc, const F& fn) {
        //store the current error state
        const auto initial_error_state = pc.get_error_state();

        //invoke the function; discard the result as it is not useful, optionals always return true
        fn();

        //if errors are found, consider the parsing as failed and cancel the errors
        if (pc.get_errors().size() > initial_error_state.get_error_count()) {
            pc.set_error_state(initial_error_state);
        }

        return true;
    }


    /**
     * Invokes the given function repeatedly, until it fails to parse or an error is detected
     * that starts at the parse position before the loop.
     * @param pc the parse context.
     * @param fn the parse function.
     * @return always true.
     */
    template <class ParseContext, class F>
    bool parse_loop_0(ParseContext& pc, const F& fn) {
        while (true) {
            const auto base_iterator = pc.get_iterator();
            const auto base_error_state = pc.get_error_state();

            //parse
            const bool result = fn();

            //if failed, stop the loop
            if (!result) {
                break;
            }

            //if new errors are detected and the first error starts at the base iterator, cancel the errors and stop the loop
            if (pc.get_errors().size() > base_error_state.get_error_count() && pc.get_errors()[base_error_state.get_error_count()].begin() == base_iterator) {
                pc.set_error_state(base_error_state);
                break;
            }
        }
        return true;
    }


} //namespace parserlib


#endif //PARSERLIB_PARSE_ALGORITHMS_HPP
