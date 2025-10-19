#ifndef PARSERLIB_RULE_HPP
#define PARSERLIB_RULE_HPP


#include <memory>
#include "parse_node.hpp"


namespace parserlib {


    /**
     * Placeholder for recursive grammars.
     * @param ParseContext type of parse context.
     */
    template <class ParseContext>
    class rule : public parse_node<rule<ParseContext>> {
    public:
        /** parse context state type. */
        using state_type = typename ParseContext::state_type;

        /**
         * Empty constructor.
         */
        rule() {
        }

        /**
         * Constructor from grammar expression.
         * @param t grammar expression.
         */
        template <class T, std::enable_if_t<!std::is_same_v<T, rule<ParseContext>>, bool> = true>
        rule(const T& t)
            : m_expr(create_impl(t))
        {
        }

        /**
         * A Rule is not copyable because references to rules are used for recursion.
         */
        rule(const rule&) = delete;

        /**
         * A Rule is not movable because references to rules are used for recursion.
         */
        rule(rule&&) = delete;

        /**
         * Assignment from grammar expression.
         * @param t grammar expression.
         * @return reference to this.
         */
        template <class T, std::enable_if_t<!std::is_same_v<T, rule<ParseContext>>, bool> = true>
        rule& operator = (const T& t) {
            m_expr = create_impl(t);
            return *this;
        }

        /**
         * A Rule is not copyable because references to rules are used for recursion.
         */
        rule& operator =(const rule&) = delete;

        /**
         * A Rule is not movable because references to rules are used for recursion.
         */
        rule& operator =(rule&&) = delete;

        zero_or_more_parse_node<rule_ref_parse_node<ParseContext>> operator *() noexcept { return rule_ref_parse_node<ParseContext>(*this); }
        one_or_more_parse_node<rule_ref_parse_node<ParseContext>> operator +() noexcept { return rule_ref_parse_node<ParseContext>(*this); }
        optional_parse_node<rule_ref_parse_node<ParseContext>> operator -() noexcept { return rule_ref_parse_node<ParseContext>(*this); }
        logical_and_parse_node<rule_ref_parse_node<ParseContext>> operator &() noexcept { return rule_ref_parse_node<ParseContext>(*this); }
        logical_not_parse_node<rule_ref_parse_node<ParseContext>> operator !() noexcept { return rule_ref_parse_node<ParseContext>(*this); }

        template <class Callback> auto operator [](const Callback& callback) noexcept {
            return callback_parse_node(get_parse_node_wrapper(*this), callback);
        }

        /**
         * If the rule is not left-recursive at the current position,
         * then it invokes the internal grammar expression to parse,
         * otherwise it returns a left recursion parse result.
         * @param pc parse context to use for parsing.
         * @return either true, false or left_recursion.
         */
        parse_result parse(ParseContext& pc) noexcept {
            if (pc.is_left_recursive_rule(*this)) {
                return left_recursion{this};
            }
            return parse_non_left_recursion(pc);
        }

        /**
         * If the rule is not left-recursive at the current position,
         * then it invokes the internal grammar expression to parse,
         * otherwise it returns false.
         * @param pc parse context to use for parsing.
         * @return either true or false.
         */
        parse_result parse_left_recursion_start(ParseContext& pc) noexcept {
            if (pc.is_left_recursive_rule(*this)) {
                return false;
            }
            return parse_non_left_recursion(pc);
        }

        /**
         * If the rule is not left-recursive at the current position,
         * then it invokes the internal grammar expression to parse,
         * otherwise it returns true.
         * @param pc parse context to use for parsing.
         * @return either true or false.
         */
        parse_result parse_left_recursion_continuation(ParseContext& pc, const state_type& match_start) noexcept {
            if (pc.is_left_recursive_rule(*this)) {
                return true;
            }
            return parse_non_left_recursion(pc);
        }

        /**
         * Allows a pointer to this object, since the unary operator & is used for converting the rule to a logical AND expression.
         * @return a pointer to this.
         */
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
            return std::make_unique<expr_impl<parse_node_wrapper_type<T>>>(get_parse_node_wrapper(t));
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
                case parse_result::TRUE:
                    break;
                case parse_result::LEFT_RECURSION:
                    return result;
            }

            //left recursion continuation
            while (pc.is_valid_parse_position()) {
                pc.push_rule_parse_position(*this);
                const auto start = pc.parse_position();
                parse_result result = m_expr->parse_left_recursion_continuation(pc, match_start);
                pc.pop_rule_parse_position(*this);
                switch (result.value()) {
                    case parse_result::FALSE:
                        return true;
                    case parse_result::TRUE:
                        if (pc.parse_position() == start) {
                            return true;
                        }
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


    /**
     * A parse node that represents a reference to a rule.
     * Rules are added to grammar expressions via references, in order to allow recursion.
     * @param ParseContext the parse context to use.
     */
    template <class ParseContext>
    class rule_ref_parse_node : public parse_node<rule_ref_parse_node<ParseContext>> {
    public:
        /**
         * The constructor.
         * @param rule the rule to refer to.
         */
        rule_ref_parse_node(rule<ParseContext>& rule) noexcept
            : m_rule(rule)
        {
        }

        /**
         * Invokes the relevant parse function of the rule.
         * @param pc parse context to use for parsing.
         * @return whatever the rule returns.
         */
        parse_result parse(ParseContext& pc) const noexcept {
            return m_rule.parse(pc);
        }

        /**
         * Invokes the relevant parse function of the rule.
         * @param pc parse context to use for parsing.
         * @return whatever the rule returns.
         */
        parse_result parse_left_recursion_start(ParseContext& pc) const noexcept {
            return m_rule.parse_left_recursion_start(pc);
        }

        /**
         * Invokes the relevant parse function of the rule.
         * @param pc parse context to use for parsing.
         * @return whatever the rule returns.
         */
        template <class State>
        parse_result parse_left_recursion_continuation(ParseContext& pc, const State& match_start) const noexcept {
            return m_rule.parse_left_recursion_continuation(pc, match_start);
        }

    private:
        rule<ParseContext>& m_rule;
    };


} //namespace parserlib


#endif //PARSERLIB_RULE_HPP
