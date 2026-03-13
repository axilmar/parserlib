#ifndef PARSERLIB_RULE_HPP
#define PARSERLIB_RULE_HPP


#include <map>
#include <vector>
#include "ref_parse_node.hpp"
#include "rule_parse_node.hpp"


namespace parserlib {


    template <class ParseContext>
    class rule {
    public:
        rule(const parse_node_ptr<ParseContext>& parse_node = {})
            : m_parse_node(get_or_create_rule_parse_node(parse_node.get_shared()))
        {
        }

        rule(const rule& r) = delete;
        rule(rule&& r) = delete;

        template <class Symbol>
        rule(const Symbol& symbol)
            : m_parse_node(parse_node_ptr<ParseContext>(symbol).get_shared())
        {
        }

        rule(rule& r)
            : rule(parse_node_ptr<ParseContext>(r))
        {
        }

        rule(bool result)
            : rule(parse_node_ptr<ParseContext>(result))
        {
        }

        ~rule() {
            get_ref_map().erase(this);
            get_rule_map().erase(this);
        }

        rule& operator = (const rule&) = delete;
        rule& operator = (rule&&) = delete;

        rule& operator = (const parse_node_ptr<ParseContext>& parse_node) {
            m_parse_node = get_rule_parse_node(parse_node.get_shared());
            return *this;
        }

        template <class Symbol>
        rule& operator = (const Symbol& symbol) {
            m_parse_node = parse_node_ptr<ParseContext>(symbol).get_shared();
            return *this;
        }

        rule& operator = (rule& r) {
            m_parse_node = get_rule_parse_node(r.m_parse_node);
            return *this;
        }

        rule& operator = (bool result) {
            m_parse_node = parse_node_ptr<ParseContext>(result);
            return *this;
        }

        const std::string& get_name() const {
            return m_parse_node->m_name;
        }

        void set_name(const std::string& name) const {
            m_parse_node->set_name(name);
        }

        const rule& operator [] (const std::string& name) const {
            set_name(name);
            return *this;
        }

        bool parse(ParseContext& pc) const {
            return m_parse_node->parse(pc);
        }

    public:
        using ref_map = std::map<const rule<ParseContext>*, std::vector<std::shared_ptr<ref_parse_node<ParseContext>>>>;
        using rule_map = std::map<const rule<ParseContext>*, std::shared_ptr<rule_parse_node<ParseContext>>>;

        parse_node_ptr<ParseContext> m_parse_node;

        static ref_map& get_ref_map() {
            static ref_map map;
            return map;
        }

        static rule_map& get_rule_map() {
            static rule_map map;
            return map;
        }

        std::shared_ptr<rule_parse_node<ParseContext>> get_rule_parse_node(const std::shared_ptr<parse_node<ParseContext>>& ptr) const {
            rule_map& map = get_rule_map();
            auto it = map.find(this);
            it->second->m_parse_node = ptr;
            return it->second;
        }

        std::shared_ptr<rule_parse_node<ParseContext>> get_or_create_rule_parse_node() const {
            rule_map& map = get_rule_map();
            auto it = map.find(this);
            if (it != map.end()) {
                return it->second;
            }
            std::shared_ptr<rule_parse_node<ParseContext>> result = std::make_shared<rule_parse_node<ParseContext>>();
            map[this] = result;
            return result;
        }

        std::shared_ptr<rule_parse_node<ParseContext>> get_or_create_rule_parse_node(const std::shared_ptr<parse_node<ParseContext>>& ptr) const {
            std::shared_ptr<rule_parse_node<ParseContext>> result = get_or_create_rule_parse_node();
            result->m_parse_node = ptr;
            return result;
        }

        parse_node_ptr<ParseContext> get_ref_parse_node() const {
            std::shared_ptr<ref_parse_node<ParseContext>> result = std::make_shared<ref_parse_node<ParseContext>>(get_or_create_rule_parse_node());
            ref_map& map = get_ref_map();
            map[this].push_back(result);
            return result;
        }

        friend class parse_node_ptr<ParseContext>;
    };


    template <class ParseContext>
    parse_node_ptr<ParseContext>::parse_node_ptr(rule<ParseContext>& rule)
        : m_parse_node(rule.get_ref_parse_node().get_shared())
    {
    }


} //namespace parserlib


#endif //PARSERLIB_RULE_HPP
