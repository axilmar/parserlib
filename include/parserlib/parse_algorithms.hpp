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


    //helper function for parsing optionals;
    //if the function adds errors to the parse context,
    //then the parsing of the optional is considered as failed,
    //even if the function itself returns true.
    template <class ParseContext, class F>
    bool parse_optional_helper(ParseContext& pc, const F& fn) {
        //store the current error state
        const auto initial_error_state = pc.get_error_state();

        //invoke the function
        const bool result = fn();

        //if errors are found, consider the parsing as failed
        if (pc.get_errors().size() > initial_error_state.get_error_count()) {
            pc.set_error_state(initial_error_state);
            return false;
        }

        //no errors found, return the normal result
        return result;
    }


    /**
     * Invokes the given function.
     * @param pc the parse context.
     * @param fn the parse function.
     * @return always true.
     */
    template <class ParseContext, class F>
    bool parse_optional(ParseContext& pc, const F& fn) {
        parse_optional_helper(pc, fn);
        return true;
    }


    /**
     * Invokes the given function repeatedly, until it fails to parse.
     * @param pc the parse context.
     * @param fn the parse function.
     * @return always true.
     */
    template <class ParseContext, class F>
    bool parse_loop_0(ParseContext& pc, const F& fn) {
        while (parse_optional_helper(pc, fn)) {
        }
        return true;
    }


} //namespace parserlib


#endif //PARSERLIB_PARSE_ALGORITHMS_HPP
