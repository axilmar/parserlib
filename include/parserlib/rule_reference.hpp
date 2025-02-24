#ifndef PARSERLIB_RULE_REFERENCE_HPP
#define PARSERLIB_RULE_REFERENCE_HPP


#include "parser.hpp"


namespace parserlib {


    template <class ParseContext> class rule;


    /**
     * A class that keeps a reference to a rule, allowing recursive rule declarations.
     * @param ParseContext parse context to use for the rule (due to indirection).
     */
    template <class ParseContext> 
    class rule_reference : public parser<rule_reference<ParseContext>> {
    public:
        /** Rule type. */
        using rule_type = rule<ParseContext>;

        /**
         * The constructor.
         * @param rule the rule to refer to.
         */
        rule_reference(const rule_type& rule)
            : m_rule(rule)
        {
        }

        /**
         * Passes the call to the rule.
         * @param context the parse context.
         * @return true if parsing succeeds, false if it fails.
         */
        bool parse(ParseContext& context) const {
            return m_rule.parse(context);
        }

        /**
         * Passes the call to the rule.
         * @param context the parse context.
         * @return true if parsing succeeds, false if it fails.
         */
        bool parse_left_recursion_start(ParseContext& context) const {
            return m_rule.parse_left_recursion_start(context);
        }

        /**
         * Passes the call to the rule.
         * @param context the parse context.
         * @param match_start_state parse state that signifies the start of a match.
         * @return true if parsing succeeds, false if it fails.
         */
        bool parse_left_recursion_continuation(ParseContext& context, const typename ParseContext::state& match_start_state) const {
            return m_rule.parse_left_recursion_continuation(context, match_start_state);
        }

    private:
        const rule_type& m_rule;
    };


} //namespace parserlib


#endif //PARSERLIB_RULE_REFERENCE_HPP
