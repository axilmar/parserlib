#ifndef PARSERLIB_PARSE_LEFT_RECURSION_ALGORITHM_HPP
#define PARSERLIB_PARSE_LEFT_RECURSION_ALGORITHM_HPP


#include <stdexcept>
#include "parse_node_id_type.hpp"
#include "left_recursion_state_type.hpp"
#include "left_recursion_exception.hpp"


namespace parserlib {


    /**
     * Implements the left recursion parsing algorithm.
     */
    class parse_left_recursion_algorithm {
    public:
        /**
         * Parses using the left recursion parsing algorithm.
         * @param context the parse context to use.
         * @param parse_node_id id of the parse node.
         * @param node the node to invoke for parsing.
         * @return true if the source was parsed successfully, false otherwise.
         * @exception left_recursion_exception thrown if the left recursion could not be resolved.
         */
        template <class ParseContext, class ParseNode> 
        static bool parse(ParseContext& context, parse_node_id_type parse_node_id, const ParseNode& node) {
            //get the left recursion state of the parse node 
            const auto [state_type, is_left_recursive] = context.get_left_recursion_state(parse_node_id);

            //parse non-left recursion
            if (!is_left_recursive) {
                return parse_non_left_recursion_state(context, parse_node_id, node);
            }

            //parse left recursion
            return parse_left_recursion_state(context, parse_node_id, state_type);
        }

    private:
        //parses no left recursion
        template <class ParseContext, class ParseNode> 
        static bool parse_non_left_recursion_state(ParseContext& context, parse_node_id_type parse_node_id, const ParseNode& node) {
            //save the current left recursion state of the node and set it to no left recursion
            context.begin_no_left_recursion_state(parse_node_id);

            //parse
            try {
                //invoke the node to parse
                const bool result = node.parse(context);

                //restore the parse node left recursion state
                context.restore_left_recursion_state(parse_node_id);

                return result;
            }

            //handle left recursion
            catch (left_recursion_exception ex) {
                context.restore_left_recursion_state(parse_node_id);

                //if the left recursion was for the current node, then handle it
                if (ex.get_parse_node_id() == parse_node_id) {
                    return do_left_recursion(context, parse_node_id, node);
                }

                //else propagate the left recursion to outter execution contexts
                throw ex;
            }

            //for other exception, restore state and rethrow it
            catch (...) {
                context.restore_left_recursion_state(parse_node_id);
                throw;
            }
        }

        //parses left recursion
        template <class ParseContext>
        static bool parse_left_recursion_state(ParseContext& context, parse_node_id_type parse_node_id, left_recursion_state_type state_type) {
            switch (state_type) {
                //start left recursion parsing
                case left_recursion_state_type::no_left_recursion:
                    throw left_recursion_exception(parse_node_id);

                //reject left recursion to allow non-left recursive branches to parse
                case left_recursion_state_type::reject_left_recursion:
                    return false;

                //accept left recursion; status set to accepted
                case left_recursion_state_type::accept_left_recursion:
                    context.accept_left_recursion_state(parse_node_id);
                    return true;

                //already accepted left recursion
                case left_recursion_state_type::accepted_left_recursion:
                    return true;
            }

            //invalid state; perhaps data corruption
            throw std::runtime_error("parse_left_recursion_algorithm::parse_left_recursion_state: invalid parse node state type.");
        }

        //do left recursion for the given parse node; first the reject state, to allow non-left recursive branches to parse,
        //then the accept phase, to allow parts after the left-recursive branches to be parsed
        template <class ParseContext, class ParseNode> 
        static bool do_left_recursion(ParseContext& context, parse_node_id_type parse_node_id, const ParseNode& node) {
            if (do_reject_left_recursion(context, parse_node_id, node)) {
                return do_accept_left_recursion(context, parse_node_id, node);
            }
            return false;
        }

        //do reject left recursion for the given parse node
        template <class ParseContext, class ParseNode> 
        static bool do_reject_left_recursion(ParseContext& context, parse_node_id_type parse_node_id, const ParseNode& node) {
            //save the match start state so as that the accept phase starts from the correct match start state
            context.save_match_start_state();

            //enter the reject left recursion state 
            context.begin_reject_left_recursion_state(parse_node_id);

            //try to parse
            try {
                const bool result = node.parse(context);
                context.restore_left_recursion_state(parse_node_id);
                context.restore_match_start_state();
                return result;
            }

            //in case of exception
            catch (...) {
                context.restore_left_recursion_state(parse_node_id);
                context.restore_match_start_state();
                throw;
            }
        }

        //do accept left recursion for the given parse node
        template <class ParseContext, class ParseNode> 
        static bool do_accept_left_recursion(ParseContext& context, parse_node_id_type parse_node_id, const ParseNode& node) {
            //parse continuously the repeating part until no more possible parsing
            for (;;) {
                //try to parse
                try {
                    //save the current match start state in order to later restore it for the next loop
                    context.save_match_start_state();
                    context.begin_accept_left_recursion_state(parse_node_id);
                    
                    //parse
                    const bool result = node.parse(context);
                    
                    //restore the state
                    context.restore_left_recursion_state(parse_node_id);                    
                    context.restore_match_start_state();

                    if (!result) {
                        break;
                    }
                }

                //on exception, restore state and rethrow
                catch (...) {
                    context.restore_left_recursion_state(parse_node_id);
                    context.restore_match_start_state();
                    throw;
                }
            }

            //success
            return true;
        }

    };


} //namespace parserlib


#endif //PARSERLIB_PARSE_LEFT_RECURSION_ALGORITHM_HPP
