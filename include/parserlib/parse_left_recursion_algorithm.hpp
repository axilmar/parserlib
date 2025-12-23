#ifndef PARSERLIB_PARSE_LEFT_RECURSION_ALGORITHM_HPP
#define PARSERLIB_PARSE_LEFT_RECURSION_ALGORITHM_HPP


#include <memory>
#include <stdexcept>
#include "left_recursion_status.hpp"
#include "left_recursion_exception.hpp"


namespace parserlib {


    /**
     * Class with the implementation of the left recursion parsing algorithm.
     */
    class parse_left_recursion_algorithm {
    public:
        /**
         * Parses a possible left recursion.
         * @param pc the parse context to use.
         * @param pn the parse node that will be invoked for parsing.
         * @return true on success, false on failure.
         * @exception left_recursion_exception thrown if the left recursion could not be resolved.
         */
        template <class ParseContext, class ParseNode> 
        static bool parse(ParseContext& pc, const ParseNode& pn) {
            //get the left recursion state for the given parse node
            auto& lrs = pc.get_left_recursion_state(std::addressof(pn));

            //if the parse iterator has advanced from the last time, enter a new no-left-recursion state
            if (lrs.get_iterator() != pc.get_iterator()) {
                return parse_no_left_recursion(pc, pn, lrs);
            }

            //since the iterator kept for the given parse node remains the same as the 
            //current iterator of the parse context, we have a left recursion; 
            //handle it according to status
            switch (lrs.get_status()) {
                //current status is no left recursion, therefore initiate left recursion handling;
                //throw an exception instead of handling it here in order to avoid failure 
                //due to following parse nodes that must parse successfully.
                case left_recursion_status::no_left_recursion:
                    throw left_recursion_exception(std::addressof(pn));

                //reject the left recursion to allow non-left recursive parts of the grammar to parse.
                case left_recursion_status::reject_left_recursion:
                    return false;

                //accept the left recursion without parsing, and unblock parsing for terminals.
                case left_recursion_status::accept_left_recursion:
                    pc.unblock_parsing();
                    return true;
            }

            //if execution reaches this point, then there is memory corruption,
            //because the status value would be invalid
            throw std::runtime_error("parse_left_recursion_algorithm: parse_left_recursion: memory corruption: invalid left recursion status.");
        }

    private:
        //parse with a specific left recursion status
        template <class ParseContext, class ParseNode, class LeftRecursionState> 
        static bool parse_left_recursion_status(ParseContext& pc, const ParseNode& pn, LeftRecursionState& lrs, left_recursion_status status) {
            //keep current state in order to restore it later
            const LeftRecursionState prev_state = lrs;

            //set new state
            lrs = LeftRecursionState(pc.get_iterator(), status);

            //try to parse
            try {
                const bool result = pn.parse(pc);

                //restore the state
                lrs = prev_state;

                //return the parse result
                return result;
            }

            //on exception, restore the parse state
            catch (...) {
                lrs = prev_state;
                throw;
            }
        }

        //parse the no left recursion status; 
        //in this status, left recursion exceptions are caught and handled appropriately.
        template <class ParseContext, class ParseNode, class LeftRecursionState> 
        static bool parse_no_left_recursion(ParseContext& pc, const ParseNode& pn, LeftRecursionState& lrs) {
            try {
                return parse_left_recursion_status(pc, pn, lrs, left_recursion_status::no_left_recursion);
            }
            catch (const left_recursion_exception& ex) {
                if (ex.get_parse_node() == std::addressof(pn)) {
                    return handle_left_recursion(pc, pn, lrs);
                }
                throw;
            }
        }

        template <class ParseContext, class ParseNode, class LeftRecursionState> 
        static bool handle_left_recursion(ParseContext& pc, const ParseNode& pn, LeftRecursionState& lrs) {
            //get the match parse state to later activate it for the accept phase,
            //in order to maintain left associativity.
            const auto match_parse_state = pc.get_match_parse_state();

            //try the reject phase
            if (!parse_left_recursion_status(pc, pn, lrs, left_recursion_status::reject_left_recursion)) {
                return false;
            }

            //try the accept phase; loop until no more parsing can be done
            for (;;) {
                //activate the match parse state, so as that following matches
                //start from the correct point
                pc.set_match_parse_state(match_parse_state);

                //block terminal parsing; before any terminal is parsed,
                //there must be an 'accept'.
                pc.block_parsing();

                //parse the accept phase
                if (!parse_left_recursion_status(pc, pn, lrs, left_recursion_status::accept_left_recursion)) {
                    pc.unblock_parsing();
                    break;
                }
            }

            //successfully parsed a left recursion
            return true;
        }
    };


} //namespace parserlib


#endif //PARSERLIB_PARSE_LEFT_RECURSION_ALGORITHM_HPP
