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

        rule(const rule& r) noexcept
            : m_parser(r.m_parser ? r.m_parser->clone() : nullptr)
        {
        }

        rule(rule&& r) noexcept
            : m_parser(std::move(r.m_parser))
        {
        }

        rule(rule& r) noexcept
            : m_parser(std::make_unique<parser_implementation<ParseContext, rule_reference<ParseContext>>>(r))
        {
        }

        template <class Parser>
        rule(const Parser& parser) noexcept
            : m_parser(std::make_unique<parser_implementation<ParseContext, Parser>>(parser))
        {
        }

        rule& operator = (const rule& r) noexcept {
            assert(&r != this);
            m_parser = r.m_parser ? r.m_parser->clone() : nullptr;
            return *this;
        }

        rule& operator = (rule&& r) noexcept {
            m_parser = std::move(r.m_parser);
            return *this;
        }

        rule& operator = (rule& r) noexcept {
            assert(&r != this);
            m_parser = std::make_unique<parser_implementation<ParseContext, rule_reference<ParseContext>>>(r);
            return *this;
        }

        template <class Parser>
        rule& operator = (const Parser& parser) noexcept {
            m_parser = std::make_unique<parser_implementation<ParseContext, Parser>>(parser);
            return *this;
        }

        bool parse(ParseContext& context) const noexcept {
            if (context.is_rule_left_recursive(*this)) {
                throw left_recursion();
            }
            else {
                parse_non_left_recursion(context);
            }
        }

        bool parse_left_recursion_start(ParseContext& context) const noexcept {
            if (context.is_rule_left_recursive(*this)) {
                return false;
            }
            else {
                parse_non_left_recursion(context);
            }
        }

        bool parse_left_recursion_continuation(ParseContext& context, const typename ParseContext::state& match_start_state) const noexcept {
            if (context.is_rule_left_recursive(*this)) {
                return true;
            }
            else {
                parse_non_left_recursion(context);
            }
        }

    private:
        struct left_recursion {
        };

        parser_ptr_type m_parser;

        template <class ParseContext>
        bool parse_non_left_recursion(ParseContext& context) const noexcept {
            const auto state = context.get_state();
            context.push_rule_parse_position(*this);
            bool result;
            try {
                result = m_parser->parse(context);
            }
            catch (left_recursion) {
                context.set_state(state);
                return parse_left_recursion(context);
            }
            context.pop_rule_parse_position(*this);
            return result;
        }

        template <class ParseContext>
        bool parse_left_recursion(ParseContext& context) const noexcept {
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

