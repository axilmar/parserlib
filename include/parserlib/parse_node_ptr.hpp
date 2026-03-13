#ifndef PARSERLIB_PARSE_NODE_PTR_HPP
#define PARSERLIB_PARSE_NODE_PTR_HPP


#include <memory>
#include "parse_node.hpp"


namespace parserlib {


    template <class ParseContext>
    class rule;


    template <class ParseContext>
    class parse_node_ptr {
    public:
        parse_node_ptr() {
        }

        template <class ParseNode>
        parse_node_ptr(const std::shared_ptr<ParseNode>& parse_node)
            : m_parse_node(parse_node)
        {
        }

        template <class Symbol>
        parse_node_ptr(const Symbol& symbol);

        template <class Symbol>
        parse_node_ptr(const Symbol* string);

        parse_node_ptr(rule<ParseContext>& rule);

        parse_node_ptr(bool result);

        explicit operator bool() const {
            return (bool)m_parse_node;
        }

        parse_node<ParseContext>* get() const {
            return m_parse_node.get();
        }

        parse_node<ParseContext>* operator ->() const {
            return m_parse_node.get();
        }

        const std::shared_ptr<parse_node<ParseContext>>& get_shared() const {
            return m_parse_node;
        }

        const std::string& get_name() const {
            return m_parse_node->m_name;
        }

        void set_name(const std::string& name) const {
            m_parse_node->set_name(name);
        }

        const parse_node_ptr& operator [] (const std::string& name) const {
            set_name(name);
            return *this;
        }

        bool parse(ParseContext& pc) const {
            return m_parse_node->parse(pc);
        }

    private:
        std::shared_ptr<parse_node<ParseContext>> m_parse_node;
    };


} //namespace parserlib


#endif //PARSERLIB_PARSE_NODE_PTR_HPP
