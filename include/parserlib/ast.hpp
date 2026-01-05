#ifndef PARSERLIB_AST_HPP
#define PARSERLIB_AST_HPP


#include <memory>
#include <list>
#include <stdexcept>
#include <algorithm>
#include "match.hpp"


namespace parserlib {


    /**
     * An Abstract Syntax Tree (AST) node.
     * @param Id id type.
     * @param Iterator iterator type.
     */
    template <class Id, class Iterator>
    class ast_node 
        : public source_range<Id, Iterator>
        , public std::enable_shared_from_this<ast_node<Id, Iterator>>
    {
    public:
        /** Id type. */
        using id_type = Id;

        /** Iterator type. */
        using iterator_type = Iterator;

        /** Ast node type. */
        using ast_node_type = ast_node<Id, Iterator>;

        /** Ast node pointer type. */
        using ast_node_ptr = std::shared_ptr<ast_node_type>;

        /** Ast container type. */
        using ast_container_type = std::list<ast_node_ptr>;

        /**
         * The constructor.
         * @param id id of the range.
         * @param begin begin iterator.
         * @param end end iterator.
         */
        ast_node(const id_type& id, const iterator_type& begin, const iterator_type& end)
            : source_range<Id, Iterator>(id, begin, end)
        {
        }

        /**
         * The destructor.
         * Virtual due to possible polymorphism.
         */
        virtual ~ast_node() {
        }

        /**
         * Returns the parent.
         * @return the parent.
         */ 
        ast_node_ptr get_parent() const {
            return m_parent ? m_parent->shared_from_this() : nullptr;
        }

        /**
         * Returns the first child.
         * @return the first child.
         */
        ast_node_ptr get_first_child() const {
            return m_children.empty() ? nullptr : m_children.front();
        }

        /**
         * Returns the last child.
         * @return the last child.
         */
        ast_node_ptr get_last_child() const {
            return m_children.empty() ? nullptr : m_children.back();
        }

        /**
         * Returns the previous sibling.
         * @return the previous sibling.
         */
        ast_node_ptr get_prev_sibling() const {
            return m_parent && m_parent->m_children.front().get() != this ? *std::prev(m_it) : nullptr;
        }

        /**
         * Returns the next sibling.
         * @return the next sibling.
         */
        ast_node_ptr get_next_sibling() const {
            return m_parent && m_parent->m_children.back().get() != this ? *std::next(m_it) : nullptr;
        }

        /**
         * Returns the root node of this tree.
         * @return the root node of this tree.
         */
        ast_node_ptr get_root() const {
            ast_node_type* node = const_cast<ast_node_type*>(this);
            for (; node->m_parent; node = node->m_parent) {
            }
            return node->shared_from_this();
        }

        /**
         * Returns the children nodes.
         * @return the children nodes.
         */ 
        const ast_container_type& get_children() const {
            return m_children;
        }

        /**
         * Adds a child node.
         * @param child child node to add.
         * @param next optional next node.
         * @exception std::invalid_argument thrown if child or next belongs to another parent.
         */
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

        /**
         * Removes a child.
         * @param child child to remove.
         * @exception std::invalid_argument thrown if the given child is not a child of this.
         */
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


    /**
     * Helper function for creating children of an AST node, with the help of a factory.
     * @param parent_node the parent node.
     * @param parent_match the parent match.
     * @param factory the AST factory.
     */
    template <class Id, class Iterator, class ASTFactory>
    void make_ast_node_children(const std::shared_ptr<ast_node<Id, Iterator>>& parent_node, const match<Id, Iterator>& parent_match, ASTFactory& factory) {
        for (const match<Id, Iterator>& child_match : parent_match.get_children()) {
            std::shared_ptr<ast_node<Id, Iterator>> child = make_ast_node(child_match, factory);
            parent_node->add_child(child);
        }
    }


    /**
     * The default AST factory.
     * It creates AST nodes without any extensions.
     * Specializations of this can create AST node subclasses.
     */
    class default_ast_factory {
    public:
        /**
         * Creates an AST node.
         * @return the created AST node.
         */
        template <class Id, class Iterator>
        std::shared_ptr<ast_node<Id, Iterator>> operator ()(const match<Id, Iterator>& m) {
            return std::make_shared<ast_node<Id, Iterator>>(m.get_id(), m.begin(), m.end());
        }
    };


    /**
     * Creates an AST for the given match and its children, using the given AST factory.
     * @param m the match to create an AST of.
     * @param the factory to use for creating an AST node.
     * @return an AST for the given match.
     */
    template <class Id, class Iterator, class ASTFactory>
    std::shared_ptr<ast_node<Id, Iterator>> make_ast_node(const match<Id, Iterator>& m, ASTFactory& factory) {
        std::shared_ptr<ast_node<Id, Iterator>> node = factory(m);
        make_ast_node_children(node, m, factory);
        return node;
    }


    /**
     * Creates an AST for the given match and its children, using the default AST factory.
     * @param m the match to create an AST of.
     * @param the factory to use for creating an AST node.
     * @return an AST for the given match.
     */
    template <class Id, class Iterator>
    std::shared_ptr<ast_node<Id, Iterator>> make_ast_node(const match<Id, Iterator>& m) {
        default_ast_factory factory;
        return make_ast_node(m, factory);
    }


} //namespace parserlib


#endif //PARSERLIB_AST_HPP
