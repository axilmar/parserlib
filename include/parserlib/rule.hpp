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

        template <class Symbol>
        rule(const Symbol& symbol)
            : m_parse_node(parse_node_ptr<ParseContext>(symbol).get_shared())
        {
        }

        rule(rule& r)
            : rule(parse_node_ptr<ParseContext>(r))
        {
        }

        ~rule() {
            get_ref_map().erase(this);
            get_rule_map().erase(this);
        }

        bool parse(ParseContext& pc) const {
            return m_parse_node->parse(pc);
        }

    public:
        using ref_map = std::map<const rule<ParseContext>*, std::vector<std::shared_ptr<ref_parse_node<ParseContext>>>>;
        using rule_map = std::map<const rule<ParseContext>*, std::shared_ptr<rule_parse_node<ParseContext>>>;

        std::shared_ptr<parse_node<ParseContext>> m_parse_node;

        static ref_map& get_ref_map() {
            static ref_map map;
            return map;
        }

        static rule_map& get_rule_map() {
            static rule_map map;
            return map;
        }

        std::shared_ptr<rule_parse_node<ParseContext>> get_or_create_rule_parse_node(const std::shared_ptr<parse_node<ParseContext>>& ptr = {}) const {
            rule_map& map = get_rule_map();
            auto it = map.find(this);
            if (it != map.end()) {
                it->second->m_parse_node = ptr;
                return it->second;
            }
            std::shared_ptr<rule_parse_node<ParseContext>> result = std::make_shared<rule_parse_node<ParseContext>>(ptr);
            map[this] = result;
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
