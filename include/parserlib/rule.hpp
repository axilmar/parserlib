#ifndef PARSERLIB_RULE_HPP
#define PARSERLIB_RULE_HPP


#include <stdexcept>
#include "parse_node.hpp"
#include "rule_left_recursion_exception.hpp"


namespace parserlib {


    class rule : public parse_node {
    public:
        using parse_node::parse_node;

        rule(rule& r) : parse_node(r) {
        }

        using parse_node::operator =;

        rule& operator = (rule& r) {
            parse_node::operator = (r);
            return *this;
        }

        bool parse(interface::parse_context& pc) {
            if (!pc.is_rule_left_recursive_at_current_parse_position(*this)) {
                return parse_non_left_recursion(pc);
            }
            else {
                auto rs = pc.get_rule_status(*this);
                switch (rs) {
                    case rule_status::none:
                        throw rule_left_recursion_exception(*this);

                    case rule_status::reject_left_recursion:
                        return false;

                    case rule_status::accept_left_recursion:
                        pc.end_accept_left_recursion();
                        return true;
                }
            }
            throw std::runtime_error("parserlib::rule::parse: invalid rule state.");
        }

    private:
        bool parse_non_left_recursion(interface::parse_context& pc) {
            try {
                return parse_with_rule_status(pc, rule_status::none);
            }
            catch (const rule_left_recursion_exception& ex) {
                if (std::addressof(ex.get_rule()) == this) {
                    return parse_left_recursion(pc);
                }
                throw ex;
            }
        }

        bool parse_left_recursion(interface::parse_context& pc) {
            if (parse_left_recursion_with_match_start(pc, rule_status::reject_left_recursion)) {
                while (parse_accept_left_recursion(pc)) {
                }
                return true;
            }
            return false;
        }

        bool parse_accept_left_recursion(interface::parse_context& pc) {
            pc.begin_accept_left_recursion();
            try {
                return parse_left_recursion_with_match_start(pc, rule_status::accept_left_recursion);
            }
            catch (...) {
                pc.end_accept_left_recursion();
                throw;
            }
        }

        bool parse_left_recursion_with_match_start(interface::parse_context& pc, rule_status rs) {
            pc.push_match_start_state();
            try {
                const bool result = parse_with_rule_status(pc, rs);
                pc.restore_match_start_state();
                return result;
            }
            catch (...) {
                pc.pop_match_start_state();
                throw;
            }
        }

        bool parse_with_rule_status(interface::parse_context& pc, rule_status rs) {
            pc.push_rule_state(*this, rs);
            try {
                const bool result = parse_node::parse(pc);
                pc.pop_rule_state(*this);
                return result;
            }
            catch (...) {
                pc.pop_rule_state(*this);
                throw;
            }
        }
    };


    bool rule_ref_parse_node::parse(interface::parse_context& pc) const {
        return m_rule.parse(pc);
    }


} //namespace parserlib


#endif //PARSERLIB_RULE_HPP
