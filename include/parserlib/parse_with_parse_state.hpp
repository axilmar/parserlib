#ifndef PARSERLIB_PARSE_WITH_PARSE_STATE_HPP
#define PARSERLIB_PARSE_WITH_PARSE_STATE_HPP


namespace parserlib {


    /**
     * Helper function used for invoking a parse function
     * by saving the parse state of the given parse context,
     * invoking the function,
     * and then restoring the parse state of given parse context,
     * either accepting the new parse state, if the parse was successful,
     * or rejecting the new parse state, if the parse was unsucessful.
     * It also captures any exception thrown during parsing
     * and restores the parse state to its previous state.
     * @param pc the parse context to use.
     * @param fn the function to invoke.
     * @return true on success, false on failure.
     */
    template <class ParseContext, class F>
    bool parse_with_parse_state(ParseContext& pc, const F& fn) {
        pc.save_parse_state();
        try {
            if (fn(pc)) {
                pc.accept_parse_state();
                return true;
            }
            pc.reject_parse_state();
            return false;
        }
        catch (...) {
            pc.reject_parse_state();
            throw;
        }
    }


    /**
     * Helper function used for invoking a parse function
     * by saving the parse state of the given parse context,
     * invoking the function,
     * and then restoring the parse state of given parse context.
     * It also captures any exception thrown during parsing
     * and restores the parse state to its previous state.
     * @param pc the parse context to use.
     * @param fn the function to invoke.
     * @return true on success, false on failure.
     */
    template <class ParseContext, class F>
    bool parse_without_parse_state(ParseContext& pc, const F& fn) {
        pc.save_parse_state();
        try {
            const bool result = fn(pc);
            pc.reject_parse_state();
            return result;
        }
        catch (...) {
            pc.reject_parse_state();
            throw;
        }
    }


} //namespace parserlib


#endif //PARSERLIB_PARSE_WITH_PARSE_STATE_HPP
