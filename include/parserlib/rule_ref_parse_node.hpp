#ifndef PARSERLIB_RULE_REF_PARSE_NODE_HPP
#define PARSERLIB_RULE_REF_PARSE_NODE_HPP


#include "parse_node.hpp"


namespace parserlib {


    template <class ParseContext> class rule;


    /**
     * Keeps a rule reference.
     * @param ParseContext type of parse context to use.
     */
    template <class ParseContext>
    class rule_ref_parse_node : public parse_node<rule_ref_parse_node<ParseContext>> {
    public:
        /**
         * The constructor.
         * @param r reference to rule.
         */
        rule_ref_parse_node(rule<ParseContext>& r) : m_rule(r) {
        }

        /**
         * Invokes the rule to parse.
         * @param pc the current parse context.
         * @return true on success, false on failure.
         */
        bool parse(ParseContext& pc) const {
            return m_rule.parse(pc);
        }

    private:
        rule<ParseContext>& m_rule;
    };


} //namespace parserlib


#endif //PARSERLIB_RULE_REF_PARSE_NODE_HPP
