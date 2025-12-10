#ifndef PARSERLIB_RULE_HPP
#define PARSERLIB_RULE_HPP


#include <stdexcept>
#include "parse_node.hpp"
#include "rule_left_recursion_exception.hpp"


namespace parserlib {


    class rule {
    public:
        // constructors

        rule() {
        }

        rule(const rule&) = delete;

        rule(rule&&) = delete;

        rule(const parse_node& pn) : m_parse_node(pn) {
        }

        rule(parse_node&& pn) : m_parse_node(std::move(pn)) {
        }

        rule(rule& r) : m_parse_node(r) {
        }

        // assignment

        rule& operator = (const rule&) = delete;

        rule& operator = (rule&&) = delete;

        rule& operator = (const parse_node& pn) {
            m_parse_node = pn;
            return *this;
        }

        rule& operator = (parse_node&& pn) {
            m_parse_node = std::move(pn);
            return *this;
        }

        rule& operator = (rule& r) {
            m_parse_node = r;
            return *this;
        }

        // DSL

        parse_node operator *() {
            return *this;
        }

        parse_node operator +() {
            return *this;
        }

        parse_node operator -() {
            return *this;
        }

        parse_node operator &() {
            return *this;
        }

        parse_node operator !() const;

        // parse

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
        parse_node m_parse_node;

        bool parse_non_left_recursion(interface::parse_context& pc) {
            try {
                return parse(pc, rule_status::none);
            }
            catch (const rule_left_recursion_exception& ex) {
                if (std::addressof(ex.get_rule()) == this) {
                    return parse_left_recursion(pc);
                }
                throw ex;
            }
        }

        bool parse_left_recursion(interface::parse_context& pc) {
            //save the left recursion match start for later
            pc.push_match_start_state();

            //the reject phase
            try {
                if (!parse(pc, rule_status::reject_left_recursion)) {
                    pc.pop_match_start_state();
                    return false;
                }
            }
            catch (...) {
                pc.pop_match_start_state();
                throw;
            }

            //the accept phase
            for (;;) {
                //activate the saved match start start for the left recursion
                //by popping it from the stack
                pc.pop_match_start_state();

                //disallow terminals until a rule accepts
                pc.begin_accept_left_recursion();

                try {
                    //save the left recursion match start state to the stack for next loop
                    pc.push_match_start_state();

                    const bool result = parse(pc, rule_status::accept_left_recursion);

                    //reactivate the appropriate match start state for the next loop
                    pc.pop_match_start_state();

                    //allow terminal matching
                    pc.end_accept_left_recursion();

                    //failure to parse more of this left recursion;
                    //end left recursion parsing
                    if (!result) {
                        break;
                    }
                }
                catch (...) {
                    pc.pop_match_start_state();
                    pc.end_accept_left_recursion();
                    throw;
                }
            }

            pc.pop_match_start_state();
            return true;
        }

        bool parse(interface::parse_context& pc, rule_status rs) {
            pc.push_rule_state(*this, rs);
            try {
                const bool result = m_parse_node.parse(pc);
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
