#ifndef PARSERLIB_AST_HPP
#define PARSERLIB_AST_HPP


#include <memory>
#include <vector>
#include <utility>
#include "parse_context.hpp"


namespace parserlib {


    template <class NodeId, class Iterator> 
    class ast_node;


    template <class T>
    const char* get_id_name(T id) noexcept {
        return nullptr;
    }
    
    
    /**
     * AST node pointer type.
     * AST nodes are managed via shared pointers.
     * @param NodeId node id.
     * @param Iterator source iterator.
     */
    template <class NodeId, class Iterator>
    using ast_node_ptr_type = std::shared_ptr<ast_node<NodeId, Iterator>>;


    /**
     * AST node container type.
     * A vector of AST node pointers.
     * @param NodeId node id.
     * @param Iterator source iterator.
     */
    template <class NodeId, class Iterator>
    using ast_node_container_type = std::vector<ast_node_ptr_type<NodeId, Iterator>>;


    /**
     * AST Node.
     * @param NodeId node id.
     * @param Iterator source iterator.
     */
    template <class NodeId, class Iterator>
    class ast_node : public std::enable_shared_from_this<ast_node<NodeId, Iterator>> {
    public:
        /** Node id type. */
        using node_id_type = NodeId;

        /** Iterator type. */
        using iterator_type = Iterator;

        /**
         * Constructor.
         * @param node_id id of node.
         * @param begin start in source code.
         * @param end end in source code.
         * @param children children nodes.
         */
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

        /**
         * The destructor.
         * It is virtual in order to allow subclassing.
         */
        virtual ~ast_node() {
        }

        /**
         * Returns the id of the node.
         * @return the id of the node.
         */
        const node_id_type& id() const noexcept {
            return m_id;
        }

        /**
         * Returns an iterator to the beginning of the portion of the source that this AST node is for.
         * @return an iterator to the beginning of the portion of the source that this AST node is for.
         */
        const iterator_type& begin() const noexcept {
            return m_begin;
        }

        /**
         * Returns an iterator to the end of the portion of the source that this AST node is for.
         * @return an iterator to the end of the portion of the source that this AST node is for.
         */
        const iterator_type& end() const noexcept {
            return m_end;
        }

        /**
         * Returns a pointer to the parent node.
         * @return a pointer to the parent node or nullptr if there is no parent.
         */
        ast_node_ptr_type<node_id_type, iterator_type> parent() const noexcept {
            return m_parent ? m_parent->shared_from_this() : nullptr;
        }

        /**
         * Returns the node's children.
         * Returns the node's children.
         */
        const ast_node_container_type<node_id_type, iterator_type>& children() const noexcept {
            return m_children;
        }

        /**
         * Returns a container with the source that corresponds to this node.
         * @return a container with the source that corresponds to this node; 
            an std::basic_string_view if the source element type satisfies the string view's character requirements,
            otherwise an std::vector.
         */
        auto source() const noexcept {
            return parserlib::source(m_begin, m_end);
        }

        /**
         * Prints the AST onto the given stream.
         * @param stream destination stream.
         * @param depth tree depth.
         * @param tabSize tab size.
         */
        template <class Stream>
        void print(Stream& stream, std::size_t depth = 0, std::size_t tabSize = 4) const noexcept {
            for (std::size_t i = 0; i < depth * tabSize; ++i) {
                stream << ' ';
            }
            stream << get_id_name(m_id);
            if (m_children.empty()) {
                stream << " : " << source() << '\n';
            }
            else {
                stream << '\n';
                for (const auto& child : m_children) {
                    child->print(stream, depth + 1, tabSize);
                }
            }
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
