#ifndef PARSERLIB_RULE_REF_PARSE_NODE_HPP
#define PARSERLIB_RULE_REF_PARSE_NODE_HPP


#include <sstream>
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
        /** The rule type. */
        using rule_type = rule<ParseContext>;

        /**
         * The constructor.
         * @param r reference to rule.
         */
        rule_ref_parse_node(rule_type& r) : m_rule(r) {
        }

        /**
         * Invokes the rule to parse.
         * @param pc the current parse context.
         * @return true on success, false on failure.
         */
        bool parse(ParseContext& pc) const {
            return m_rule.parse(pc);
        }

        /**
         * Returns the rule.
         * @return the rule.
         */
        rule_type& rule() const {
            return m_rule;
        }

        #ifndef NDEBUG
        std::string text() const {
            std::stringstream stream;
            if (m_rule.name().empty()) {
                stream << m_rule.this_ptr();
            }
            else {
                stream << m_rule.name();
            }
            return stream.str();
        }
        #endif NDEBUG

    private:
        rule_type& m_rule;
    };


} //namespace parserlib


#endif //PARSERLIB_RULE_REF_PARSE_NODE_HPP
