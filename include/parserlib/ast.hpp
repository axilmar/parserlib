#ifndef PARSERLIB_AST_HPP
#define PARSERLIB_AST_HPP


#include <memory>
#include <list>
#include <stdexcept>
#include <algorithm>
#include "match.hpp"


namespace parserlib {


    template <class Iterator, class Id>
    class ast_node 
        : public source_partition<Iterator, Id>
        , public std::enable_shared_from_this<ast_node<Iterator, Id>>
    {
    public:
        using id_type = Id;
        using iterator_type = Iterator;
        using ast_node_type = ast_node<Iterator, Id>;
        using ast_node_ptr = std::shared_ptr<ast_node_type>;
        using ast_container_type = std::list<ast_node_ptr>;

        ast_node(const id_type& id, const iterator_type& begin, const iterator_type& end)
            : source_partition<Iterator, Id>(id, begin, end)
        {
        }

        virtual ~ast_node() {
        }

        ast_node_ptr get_parent() const {
            return m_parent ? m_parent->shared_from_this() : nullptr;
        }

        ast_node_ptr get_first_child() const {
            return m_children.empty() ? nullptr : m_children.front();
        }

        ast_node_ptr get_last_child() const {
            return m_children.empty() ? nullptr : m_children.back();
        }

        ast_node_ptr get_prev_sibling() const {
            return m_parent && m_parent->m_children.front().get() != this ? *std::prev(m_it) : nullptr;
        }

        ast_node_ptr get_next_sibling() const {
            return m_parent && m_parent->m_children.back().get() != this ? *std::next(m_it) : nullptr;
        }

        ast_node_ptr get_root() const {
            ast_node_type* node = const_cast<ast_node_type*>(this);
            for (; node->m_parent; node = node->m_parent) {
            }
            return node->shared_from_this();
        }

        const ast_container_type& get_children() const {
            return m_children;
        }

        void add_child(const ast_node_ptr& child, const ast_node_ptr& next = nullptr) {
            if (child->m_parent) {
                throw std::invalid_argument("child");
            }
            if (next && next->m_parent != this) {
                throw std::invalid_argument("next");
            }
            child->m_parent = this;
            const auto it = next ? std::find(m_children.begin(), m_children.end(), next) : m_children.end();
            child->m_it = m_children.insert(it, child);
        }

        void remove_child(const ast_node_ptr& child) {
            if (child->m_parent != this) {
                throw std::invalid_argument("child");
            }
            child->m_parent = nullptr;
            m_children.erase(child->m_it);
        }

    private:
        ast_node_type* m_parent{ nullptr };
        ast_container_type m_children;
        typename ast_container_type::iterator m_it;
    };


    template <class Id, class Iterator, class ASTFactory>
    void make_ast_node_children(const std::shared_ptr<ast_node<Iterator, Id>>& parent_node, const match<Iterator, Id>& parent_match, ASTFactory& factory) {
        for (const match<Iterator, Id>& child_match : parent_match.get_children()) {
            std::shared_ptr<ast_node<Iterator, Id>> child = make_ast_node(child_match, factory);
            parent_node->add_child(child);
        }
    }


    class default_ast_factory {
    public:
        template <class Id, class Iterator>
        std::shared_ptr<ast_node<Iterator, Id>> operator ()(const match<Iterator, Id>& m) {
            return std::make_shared<ast_node<Iterator, Id>>(m.get_id(), m.begin(), m.end());
        }
    };


    template <class Id, class Iterator, class ASTFactory>
    std::shared_ptr<ast_node<Iterator, Id>> make_ast_node(const match<Iterator, Id>& m, ASTFactory& factory) {
        std::shared_ptr<ast_node<Iterator, Id>> node = factory(m);
        make_ast_node_children(node, m, factory);
        return node;
    }


    template <class Id, class Iterator>
    std::shared_ptr<ast_node<Iterator, Id>> make_ast_node(const match<Iterator, Id>& m) {
        default_ast_factory factory;
        return make_ast_node(m, factory);
    }


    template <class Id, class Iterator, class ASTFactory>
    auto make_ast_nodes(const std::vector<match<Iterator, Id>>& m, ASTFactory& factory) {
        std::vector<std::shared_ptr<ast_node<Iterator, Id>>> result;
        for (const auto& match : m) {
            std::shared_ptr<ast_node<Iterator, Id>> node = factory(match);
            make_ast_node_children(node, match, factory);
            result.push_back(node);
        }
        return result;
    }


    template <class Id, class Iterator>
    auto make_ast_nodes(const std::vector<match<Iterator, Id>>& m) {
        default_ast_factory factory;
        return make_ast_nodes(m, factory);
    }


} //namespace parserlib


#endif //PARSERLIB_AST_HPP
