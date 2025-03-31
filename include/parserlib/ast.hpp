#ifndef PARSERLIB_AST_HPP
#define PARSERLIB_AST_HPP


#include <memory>
#include <vector>
#include <utility>
#include "parse_context.hpp"


namespace parserlib {


    template <class NodeId, class Iterator> 
    class ast_node;
    
    
    template <class NodeId, class Iterator>
    using ast_node_ptr_type = std::shared_ptr<ast_node<NodeId, Iterator>>;


    template <class NodeId, class Iterator>
    using ast_node_container_type = std::vector<ast_node_ptr_type<NodeId, Iterator>>;


    template <class NodeId, class Iterator>
    class ast_node : public std::enable_shared_from_this<ast_node<NodeId, Iterator>> {
    public:
        using node_id_type = NodeId;
        using iterator_type = Iterator;

        ast_node(const node_id_type& node_id, const iterator_type& begin, const iterator_type& end, ast_node_container_type<node_id_type, iterator_type>&& children) noexcept
            : m_id(node_id)
            , m_begin(begin)
            , m_end(end)
            , m_children(std::move(children))
        {
            for (const auto& child : m_children) {
                child->ast_node<node_id_type, iterator_type>::m_parent = this;
            }
        }

        virtual ~ast_node() {
        }

        const node_id_type& id() const noexcept {
            return m_id;
        }

        const iterator_type& begin() const noexcept {
            return m_begin;
        }

        const iterator_type& end() const noexcept {
            return m_end;
        }

        ast_node_ptr_type<node_id_type, iterator_type> parent() const noexcept {
            return m_parent ? m_parent->shared_from_this() : nullptr;
        }

        const ast_node_container_type<node_id_type, iterator_type>& children() const noexcept {
            return m_children;
        }

        auto source() const noexcept {
            return parserlib::source(m_begin, m_end);
        }

    private:
        const node_id_type m_id;
        const iterator_type m_begin;
        const iterator_type m_end;
        ast_node<node_id_type, iterator_type>* m_parent{ nullptr };
        const ast_node_container_type<node_id_type, iterator_type> m_children;
    };


} //namespace parserlib


#endif //PARSERLIB_AST_HPP
