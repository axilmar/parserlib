#ifndef PARSERLIB__EXAMPLES__XML__XML_AST__HPP
#define PARSERLIB__EXAMPLES__XML__XML_AST__HPP


#include <iostream>
#include "xml_grammar.hpp"


namespace xml
{


    class node
    {
    public:
        virtual ~node()
        {
        }

        virtual bool equals(const node& n) const = 0;

        virtual void to_xml(std::ostream& stream) const = 0;
    };


    typedef std::shared_ptr<node> node_ptr;


    class name : public node
    {
    public:
        name(const std::string& value)
            : m_value(value)
        {
        }

        virtual bool equals(const node& n) const
        {
            const name* av = dynamic_cast<const name*>(&n);
            return av && av->m_value == m_value;
        }

        virtual void to_xml(std::ostream& stream) const
        {
            stream << m_value;
        }

        template <typename Match>
        static std::shared_ptr<name> parse(const Match& match, std::vector<std::shared_ptr<node>> &nodes)
        {
            return std::make_shared<name>(match);
        }

    private:
        std::string m_value;
    };


    typedef std::shared_ptr<class name> name_ptr;


    name_ptr name(const std::string& n)
    {
        return std::make_shared<class name>(n);
    }


    class attribute_value : public node
    {
    public:
        attribute_value(const std::string& value)
            : m_value(value)
        {
        }

        virtual bool equals(const node& n) const
        {
            const attribute_value* av = dynamic_cast<const attribute_value*>(&n);
            return av && av->m_value == m_value;
        }

        virtual void to_xml(std::ostream& stream) const
        {
            stream << '\"' << m_value << '\"';
        }

        template <typename Match>
        static std::shared_ptr<attribute_value> parse(const Match& match, std::vector<std::shared_ptr<node>> &nodes)
        {
            return std::make_shared<attribute_value>(match);
        }

    private:
        std::string m_value;
    };


    class attribute : public node
    {
    public:
        attribute(const name_ptr& name, const std::shared_ptr<attribute_value>& value)
            : m_name(name), m_value(value)
        {

        }

        virtual bool equals(const node& n) const
        {
            const attribute* a = dynamic_cast<const attribute*>(&n);
            return a && a->m_name->equals(*m_name) && a->m_value->equals(*m_value);
        }

        void to_xml(std::ostream& stream) const
        {
            stream << ' ';
            m_name->to_xml(stream);
            stream << " = ";
            m_value->to_xml(stream);
        }

        template <typename Match>
        static std::shared_ptr<attribute> parse(const Match& match, std::vector<std::shared_ptr<node>> &nodes)
        {
            std::shared_ptr<attribute_value> value = std::dynamic_pointer_cast<attribute_value>(nodes.back());
            nodes.pop_back();
            name_ptr name = std::dynamic_pointer_cast<class name>(nodes.back());
            nodes.pop_back();
            return std::make_shared<attribute>(name, value);
        }

    private:
        name_ptr m_name;
        std::shared_ptr<attribute_value> m_value;
    };


    typedef std::shared_ptr<class attribute> attribute_ptr;


    attribute_ptr attribute(const std::string& n, const std::string& value)
    {
        return std::make_shared<class attribute>(name(n), std::make_shared<attribute_value>(value));
    }


    class element_node : public node
    {
    };


    typedef std::shared_ptr<element_node> element_node_ptr;


    class content : public element_node
    {
    public:
        content(const std::string& value)
            : m_value(value)
        {
        }

        virtual bool equals(const node& n) const
        {
            const content* c = dynamic_cast<const content*>(&n);
            return c && c->m_value == m_value;
        }

        virtual void to_xml(std::ostream& stream) const
        {
            stream << m_value;
        }

        template <typename Match>
        static std::shared_ptr<content> parse(const Match& match, std::vector<std::shared_ptr<node>> &nodes)
        {
            return std::make_shared<content>(match);
        }

    private:
        std::string m_value;
    };


    element_node_ptr content(const std::string& c)
    {
        return std::make_shared<class content>(c);
    }


    class element : public element_node
    {
    public:
        element(const name_ptr& name, const std::vector<attribute_ptr>& attributes, const std::vector<element_node_ptr>& children)
            : m_name(name), m_attributes(attributes), m_children(children)
        {
        }

        virtual bool equals(const node& n) const
        {
            const element* e = dynamic_cast<const element*>(&n);

            if (!e)
            {
                return false;
            }

            if (!e->m_name->equals(*m_name))
            {
                return false;
            }

            if (e->m_attributes.size() != m_attributes.size())
            {
                return false;
            }

            if (e->m_children.size() != m_children.size())
            {
                return false;
            }

            for (size_t i = 0; i < m_attributes.size(); ++i)
            {
                if (!e->m_attributes[i]->equals(*m_attributes[i]))
                {
                    return false;
                }
            }

            for (size_t i = 0; i < m_children.size(); ++i)
            {
                if (!e->m_children[i]->equals(*m_children[i]))
                {
                    return false;
                }
            }

            return true;
        }

        virtual void to_xml(std::ostream& stream) const
        {
            stream << '<';
            m_name->to_xml(stream);
            for (const auto& attr : m_attributes)
            {
                attr->to_xml(stream);
            }
            stream << '>';
            for (const auto& child : m_children)
            {
                child->to_xml(stream);
            }
            stream << "</";
            m_name->to_xml(stream);
            stream << '>';
        }

        template <typename Match>
        static std::shared_ptr<element> parse(const Match& match, std::vector<std::shared_ptr<node>> &nodes)
        {
            name_ptr closing_name = std::dynamic_pointer_cast<class name>(nodes.back());
            nodes.pop_back();

            std::vector<element_node_ptr> children;
            while (!nodes.empty())
            {
                element_node_ptr child = std::dynamic_pointer_cast<element_node>(nodes.back());
                if (!child) break;
                nodes.pop_back();
                children.push_back(child);
            }

            std::vector<attribute_ptr> attributes;
            while (!nodes.empty())
            {
                attribute_ptr child = std::dynamic_pointer_cast<class attribute>(nodes.back());
                if (!child) break;
                nodes.pop_back();
                attributes.push_back(child);
            }

            name_ptr opening_name = std::dynamic_pointer_cast<class name>(nodes.back());
            nodes.pop_back();

            if (!opening_name->equals(*closing_name))
            {
                throw std::logic_error("invalid xml closing tag");
            }

            return std::make_shared<element>(
                opening_name, 
                std::vector<attribute_ptr>{attributes.rbegin(), attributes.rend()}, 
                std::vector<element_node_ptr>{children.rbegin(), children.rend()});
        }

    private:
        name_ptr m_name;
        std::vector<attribute_ptr> m_attributes;
        std::vector<element_node_ptr> m_children;
    };


    typedef std::shared_ptr<class element> element_ptr;


    element_ptr element(const name_ptr& name, const std::vector<attribute_ptr>& attributes, const std::vector<element_node_ptr>& children)
    {
        return std::make_shared<class element>(name, attributes, children);
    }


    template <typename ParseContext>
    element_ptr create_ast(ParseContext& pc)
    {
        std::vector<node_ptr> nodes;

        for (const auto& match : pc.matches)
        {
            node_ptr node;

            if (match.tag == TAG_NAME)
            {
                node = name::parse(match, nodes);
            }
            else if (match.tag == TAG_ATTRIBUTE_VALUE)
            {
                node = attribute_value::parse(match, nodes);
            }
            else if (match.tag == TAG_ATTRIBUTE)
            {
                node = attribute::parse(match, nodes);
            }
            else if (match.tag == TAG_CONTENT)
            {
                node = content::parse(match, nodes);
            }
            else if (match.tag == TAG_ELEMENT)
            {
                node = element::parse(match, nodes);
            }

            nodes.push_back(node);
        }

        if (nodes.size() != 1)
        {
            throw std::logic_error("invalid ast");
        }

        return std::dynamic_pointer_cast<class element>(nodes.back());
    }


} //namespace xml


#endif //PARSERLIB__EXAMPLES__XML__XML_AST__HPP
