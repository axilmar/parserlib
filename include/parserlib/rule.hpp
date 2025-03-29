#ifndef PARSERLIB_RULE_HPP
#define PARSERLIB_RULE_HPP


#include <memory>
#include "parse_node.hpp"


namespace parserlib {


    template <class ParseContext>
    class rule : public parse_node<rule<ParseContext>> {
        using state_type = typename ParseContext::state;
    public:
        rule() {
        }

        template <class T, std::enable_if_t<!std::is_same_v<T, rule<ParseContext>>, bool> = true>
        rule(const T& t) 
            : m_expr(create_impl(t))
        {
        }

        rule(rule&&) = delete;

        template <class T, std::enable_if_t<!std::is_same_v<T, rule<ParseContext>>, bool> = true>
        rule& operator = (const T& t) {
            m_expr = create_impl(t);
            return *this;
        }

        rule& operator =(rule&&) = delete;

        parse_result parse(ParseContext& pc) noexcept {
            if (pc.is_left_recursive_rule(*this)) {
                return left_recursion{this};
            }
            return parse_non_left_recursion(pc);
        }

        parse_result parse_left_recursion_start(ParseContext& pc) noexcept {
            if (pc.is_left_recursive_rule(*this)) {
                return false;
            }
            return parse_non_left_recursion(pc);
        }

        parse_result parse_left_recursion_continuation(ParseContext& pc, const state_type& match_start) noexcept {
            if (pc.is_left_recursive_rule(*this)) {
                return true;
            }
            return parse_non_left_recursion(pc);
        }

        auto pointer() noexcept {
            return this;
        }

    private:
        class expr {
        public:
            virtual ~expr() noexcept {
            }

            virtual parse_result parse(ParseContext& pc) const noexcept = 0;
            virtual parse_result parse_left_recursion_start(ParseContext& pc) const noexcept = 0;
            virtual parse_result parse_left_recursion_continuation(ParseContext& pc, const state_type& match_start) const noexcept = 0;
        };

        template <class Parser>
        class expr_impl : public expr {
        public:
            expr_impl(const Parser& parser) noexcept
                : m_parser(parser)
            {
            }

            parse_result parse(ParseContext& pc) const noexcept override {
                return m_parser.parse(pc);
            }

            parse_result parse_left_recursion_start(ParseContext& pc) const noexcept override {
                return m_parser.parse_left_recursion_start(pc);
            }

            parse_result parse_left_recursion_continuation(ParseContext& pc, const state_type& match_start) const noexcept override {
                return m_parser.parse_left_recursion_continuation(pc, match_start);
            }

        private:
            Parser m_parser;
        };

        std::unique_ptr<expr> m_expr;

        template <class T>
        static auto create_impl(const T& t) noexcept {
            return std::make_unique<expr_impl<decltype(get_parse_node_wrapper(t))>>(get_parse_node_wrapper(t));
        }

        parse_result parse_left_recursion(ParseContext& pc) noexcept {
            const auto match_start = pc.state();

            //left recursion start
            pc.push_rule_parse_position(*this);
            parse_result result = m_expr->parse_left_recursion_start(pc);
            pc.pop_rule_parse_position(*this);
            switch (result.value()) {
                case parse_result::FALSE:
                    return false;
                case parse_result::LEFT_RECURSION:
                    return result;
            }

            //left recursion continuation
            for (;;) {
                pc.push_rule_parse_position(*this);
                parse_result result = m_expr->parse_left_recursion_continuation(pc, match_start);
                pc.pop_rule_parse_position(*this);
                switch (result.value()) {
                    case parse_result::FALSE:
                        break;
                    case parse_result::LEFT_RECURSION:
                        return result;
                }
            }

            return true;
        }

        parse_result parse_non_left_recursion(ParseContext& pc) noexcept {
            pc.push_rule_parse_position(*this);
            parse_result result = m_expr->parse(pc);
            pc.pop_rule_parse_position(*this);
            switch (result.value()) {
                case parse_result::FALSE:
                    return false;
                case parse_result::TRUE:
                    return true;
                case parse_result::LEFT_RECURSION:
                    return result.left_recursion_rule() == this ? parse_left_recursion(pc) : result;
            }
            return result;
        }
    };


    template <class ParseContext>
    class rule_ref_parse_node : public parse_node<rule_ref_parse_node<ParseContext>> {
    public:
        rule_ref_parse_node(rule<ParseContext>& rule) noexcept
            : m_rule(rule)
        {
        }

        parse_result parse(ParseContext& pc) noexcept {
            return m_rule.parse(pc);
        }

        parse_result parse_left_recursion_start(ParseContext& pc) noexcept {
            return m_rule.parse_left_recursion_start(pc);
        }

        parse_result parse_left_recursion_continuation(ParseContext& pc, const state_type& match_start) noexcept {
            return m_rule.parse_left_recursion_continuation(pc, match_start);
        }

    private:
        rule<ParseContext>& m_rule;
    };


} //namespace parserlib


#endif //PARSERLIB_RULE_HPP
