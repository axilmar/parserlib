#ifndef PARSERLIB_RULE_HPP
#define PARSERLIB_RULE_HPP


#include <cassert>
#include <utility>
#include "parser.hpp"
#include "parser_implementation.hpp"
#include "rule_reference.hpp"
#include "parse_context.hpp"


namespace parserlib {


    /**
     * A class that represents a rule for a grammar.
     * Rules are elements that can be recursive.
     * Rules are managed by reference, and therefore they are not copyable or movable. 
     * @param ParseContext the parse context that the rule uses.
     */
    template <class ParseContext = parse_context<>>
    class rule : public parser<rule<ParseContext>> {
    public:
        /** The parser interface type. */
        using parser_interface_type = parser_interface<ParseContext>;

        /** The parser interface pointer type. */
        using parser_interface_ptr_type = std::unique_ptr<parser_interface_type>;

        /**
         * The default constructor.
         * It creates an empty rule.
         */
        rule() {
        }

        /**
         * The copy constructor.
         * Deleted because rules are managed via references.
         */
        rule(const rule&) = delete;

        /**
         * The move constructor.
         * Deleted because rules are managed via references.
         */
        rule(rule&&) = delete;

        /**
         * Constructor from parser.
         * The appropriate parser wrapper is created internally.
         * @param parser the parser to create an internal parser wrapper for.
         */
        template <class Parser>
        rule(const Parser& parser)
            : m_parser(std::make_unique<parser_implementation<ParseContext, parser_wrapper_type<Parser>>>(get_parser_wrapper(parser)))
        {
        }

        /**
         * Special constructor for rules.
         * This rule will refer to the given rule through a rule reference.
         * @param r the rule to refer to by this rule.
         */
        template <class Parser, std::enable_if_t<std::is_same_v<Parser, rule>, bool> = true>
        explicit rule(Parser& r)
            : m_parser(std::make_unique<parser_implementation<ParseContext, parser_wrapper_type<rule>>>(get_parser_wrapper(r)))
        {
        }

        /**
         * The copy assignment operator.
         * Deleted because rules are managed via references.
         */
        rule& operator = (const rule& r) = delete;

        /**
         * The move assignment operator.
         * Deleted because rules are managed via references.
         */
        rule& operator = (rule&& r) = delete;

        /**
         * Sets the internal parser.
         * It creates a parser wrapper for the given parser.
         * @param parser the parser to create the parser wrapper for.
         * @return reference to this.
         */
        template <class Parser>
        rule& operator = (const Parser& parser) {
            m_parser = std::make_unique<parser_implementation<ParseContext, parser_wrapper_type<Parser>>>(get_parser_wrapper(parser));
            return *this;
        }

        /**
         * Sets the internal parser for a rule.
         * It creates a rule reference for the given rule.
         * @param r the rule to create the rule reference for.
         * @return reference to this.
         */
        template <class Parser, std::enable_if_t<std::is_same_v<Parser, rule>, bool> = true>
        rule& operator = (Parser& r) {
            m_parser = std::make_unique<parser_implementation<ParseContext, parser_wrapper_type<rule>>>(get_parser_wrapper(r));
            return *this;
        }

        /**
         * Invokes the internal parser for parsing.
         * @param context the parse context.
         * @return true on success, false on failure.
         * @exception left_recursion internal exception thrown if the rule is left-recursive at the current position;
         *  this exception does not propagate to the user, it is used internally as a non-local-goto to handle left recursions.
         */
        bool parse(ParseContext& context) const {
            if (context.is_rule_left_recursive(*this)) {
                throw left_recursion{ this };
            }
            else {
                return parse_non_left_recursion(context);
            }
        }

        /**
         * Invokes the internal parser for parsing, as the first step towards resolving left recursion.
         * @param context the parse context.
         * @return true on success, false on failure or if the rule is left-recursive in the current parse position.
         */
        bool parse_left_recursion_start(ParseContext& context) const {
            if (context.is_rule_left_recursive(*this)) {
                return false;
            }
            else {
                return parse_non_left_recursion(context);
            }
        }

        /**
         * Invokes the internal parser for parsing, as the next step, after the first one, in order to complete parse of left recursion.
         * @param context the parse context.
         * @return true on success or if the rule is left-recursive in the current parse position, false otherwise.
         */
        bool parse_left_recursion_continuation(ParseContext& context, const typename ParseContext::state& match_start_state) const {
            if (context.is_rule_left_recursive(*this)) {
                return true;
            }
            else {
                return parse_non_left_recursion(context);
            }
        }

    private:
        struct left_recursion {
            const rule* recursive_rule;
        };

        parser_interface_ptr_type m_parser;

        bool parse_non_left_recursion(ParseContext& context) const {
            const auto state = context.get_state();
            context.push_rule_parse_position(*this);
            bool result;
            try {
                result = m_parser->parse(context);
            }
            catch (left_recursion ex) {
                if (ex.recursive_rule == this) {
                    context.set_state(state);
                    return parse_left_recursion(context);
                }
                else {
                    throw;
                }
            }
            context.pop_rule_parse_position(*this);
            return result;
        }

        bool parse_left_recursion(ParseContext& context) const {
            const auto match_start_state = context.get_state();

            //parse left recursion start
            const bool result = m_parser->parse_left_recursion_start(context);
            context.pop_rule_parse_position(*this);
            if (!result) {
                return false;
            }

            //parse left recursion continuation
            for (;;) {
                context.push_rule_parse_position(*this);
                const bool result = m_parser->parse_left_recursion_continuation(context, match_start_state);
                context.pop_rule_parse_position(*this);
                if (!result) {
                    break;
                }
            }

            return true;
        }
    };


} //namespace parserlib


#endif //PARSERLIB_RULE_HPP

