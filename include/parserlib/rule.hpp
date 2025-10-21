#ifndef PARSERLIB_RULE_HPP
#define PARSERLIB_RULE_HPP


#include <memory>
#include <utility>
#include "wrapper_parser_node.hpp"
#include "parse_context.hpp"


namespace parserlib {


    template <class ParseContext> class rule;


    template <class ParseContext> class rule_reference_parser_node : public parser_node<rule_reference_parser_node<ParseContext>> {
    public:
        rule_reference_parser_node(rule<ParseContext>& r) : m_rule(r) {
        }

        bool parse(ParseContext& pc) const {
            return m_rule.parse(pc);
        }

    private:
        rule<ParseContext>& m_rule;
    };


    template <class ParseContext = parse_context<>> class rule : public parser_node<rule<ParseContext>> {
    public:
        rule() {
        }

        rule(const rule& r) : m_parser(r.m_parser) {
        }

        rule(rule&& r) : m_parser(std::move(r.m_parser)) {
        }

        rule(rule& r) : m_parser(make_parser(r)) {
        }

        template <class Parser> rule(const Parser& p)
        : m_parser(make_parser(p))
        {
        }

        rule& operator = (const rule& r) {
             m_parser = r.m_parser;
             return *this;
        }

        rule& operator = (rule&& r) {
            m_parser = std::move(r.m_parser);
            return *this;
        }

        rule& operator = (rule& r) {
            m_parser = make_parser(r);
            return *this;
        }

        template <class Parser> rule& operator = (const Parser& p) {
            m_parser = make_parser(p);
            return *this;
        }

        bool parse(ParseContext& pc) const {
            return m_parser->parse(pc);
        }

    private:
        std::shared_ptr<wrapper_parser_node_interface<ParseContext>> m_parser;

        template <class Parser> static auto make_parser(const Parser& p) {
            return std::make_shared<wrapper_parser_node_implementation<ParseContext, decltype(parser(p))>>(parser(p));
        }
    };


    template <class ParseContext> rule_reference_parser_node<ParseContext> parser(rule<ParseContext>& r) {
        return rule_reference_parser_node<ParseContext>(r);
    }


    template <class ParseContext, std::enable_if_t<sizeof(ParseContext) == 0, bool> = true>
    rule_reference_parser_node<ParseContext> parser(const rule<ParseContext>& r) {
    }


} //namespace parserlib


#endif // PARSERLIB_RULE_HPP
