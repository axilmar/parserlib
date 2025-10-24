#ifndef PARSERLIB_RULE_HPP
#define PARSERLIB_RULE_HPP


#include <memory>
#include <utility>
#include "wrapper_parser_node.hpp"
#include "parse_context.hpp"
#include "rule_reference_parser_node.hpp"


namespace parserlib {


    template <class ParseContext> rule_reference_parser_node<ParseContext> parser(const rule<ParseContext>& r) {
        return rule_reference_parser_node<ParseContext>(r);
    }


    template <class ParseContext = parse_context<>> class rule : public parser_node<rule<ParseContext>>, public rule_tag {
    public:
        rule() {
        }

        rule(const rule& r) 
            : m_parser(make_parser(r))
        {
        }

        rule(rule&& r) = delete;

        template <class Parser> rule(const Parser& p)
            : m_parser(make_parser(p))
        {
        }

        rule& operator = (const rule& r) {
             m_parser = make_parser(r);
             return *this;
        }

        rule& operator = (rule&& r) = delete;

        template <class Parser> rule& operator = (const Parser& p) {
            m_parser = make_parser(p);
            return *this;
        }

        bool parse(ParseContext& pc) const {
            return pc.parse_rule(*this);
        }

        const std::shared_ptr<wrapper_parser_node_interface<ParseContext>>& parser() const {
            return m_parser;
        }

    private:
        std::shared_ptr<wrapper_parser_node_interface<ParseContext>> m_parser;

        template <class Parser> static auto make_parser(const Parser& p) {
            using parser_wrapper_type = std::decay_t<decltype(parserlib::parser(p))>;
            return std::make_shared<wrapper_parser_node_implementation<ParseContext, parser_wrapper_type>>(parserlib::parser(p));
        }
    };


} //namespace parserlib


#endif // PARSERLIB_RULE_HPP
