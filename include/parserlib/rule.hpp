#ifndef PARSERLIB_RULE_HPP
#define PARSERLIB_RULE_HPP


#include <cassert>
#include <utility>
#include "parser.hpp"
#include "parser_implementation.hpp"
#include "rule_reference.hpp"
#include "parse_context.hpp"


namespace parserlib {


    template <class ParseContext = parse_context<>>
    class rule : public parser<rule<ParseContext>> {
    public:
        using parser_interface_type = parser_interface<ParseContext>;
        using parser_ptr_type = std::unique_ptr<parser_interface_type>;

        rule() noexcept {
        }

        rule(const rule& r) = delete;
        rule(rule&& r) = delete;

        template <class Parser>
        rule(const Parser& parser) noexcept
            : m_parser(std::make_unique<parser_implementation<ParseContext, parser_wrapper_type<Parser>>>(get_parser_wrapper(parser)))
        {
        }

        template <class Parser, std::enable_if_t<std::is_same_v<Parser, rule>, bool> = true>
        explicit rule(Parser& r) noexcept
            : m_parser(std::make_unique<parser_implementation<ParseContext, parser_wrapper_type<rule>>>(get_parser_wrapper(r)))
        {
        }

        rule& operator = (const rule& r) = delete;
        rule& operator = (rule&& r) = delete;

        template <class Parser>
        rule& operator = (const Parser& parser) noexcept {
            m_parser = std::make_unique<parser_implementation<ParseContext, parser_wrapper_type<Parser>>>(get_parser_wrapper(parser));
            return *this;
        }

        template <class Parser, std::enable_if_t<std::is_same_v<Parser, rule>, bool> = true>
        rule& operator = (Parser& r) noexcept {
            m_parser = std::make_unique<parser_implementation<ParseContext, parser_wrapper_type<rule>>>(get_parser_wrapper(r));
            return *this;
        }

        bool parse(ParseContext& context) const {
            if (context.is_rule_left_recursive(*this)) {
                throw left_recursion{ this };
            }
            else {
                return parse_non_left_recursion(context);
            }
        }

        bool parse_left_recursion_start(ParseContext& context) const {
            if (context.is_rule_left_recursive(*this)) {
                return false;
            }
            else {
                return parse_non_left_recursion(context);
            }
        }

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

        parser_ptr_type m_parser;

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

