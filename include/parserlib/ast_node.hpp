#ifndef PARSERLIB_AST_NODE_HPP
#define PARSERLIB_AST_NODE_HPP


#include <cassert>
#include <list>
#include <vector>
#include <memory>
#include "parse_definitions.hpp"
#include "match.hpp"


namespace parserlib {


    /**
     * Base class for Abstract Syntax Tree nodes.
     * @param ParseDefinitions parse definitions.
     */
    template <class ParseDefinitions = parse_definitions<>>
    class ast_node : public std::enable_shared_from_this<ast_node<ParseDefinitions>> {
    public:
        /** Input iterator type. */
        using input_iterator_type = typename ParseDefinitions::input_iterator_type;

        /** Input token type. */
        using input_token_type = typename ParseDefinitions::input_token_type;

        /** Input span type. */
        using input_span_type = span<input_iterator_type>;

        /** The output token becomes the id of ast nodes. */
        using ast_node_id_type = typename ParseDefinitions::output_token_type;

        /** AST node pointer type. */
        using ast_node_ptr_type = std::shared_ptr<ast_node>;

        /** AST node list type. */
        using ast_node_list_type = std::list<ast_node_ptr_type>;

        /**
         * The default constructor.
         */
        ast_node() noexcept {
        }

        /**
         * Constructor from parameters.
         * @param id id of node.
         * @param span span over input that this AST node corresponds to.
         */
        ast_node(const ast_node_id_type& id, const input_span_type& span) noexcept
            : m_id(id)
            , m_span(span)
        {
        }

        /**
         * Virtual destructor due to inheritance.
         */
        virtual ~ast_node() {
        }

        /**
         * Returns a pointer to the parent node.
         * @return pointer to the parent node, or null if this node is not a child node.
         */
        ast_node_ptr_type parent() const noexcept {
            return m_parent.lock();
        }

        /**
         * Returns the list of children nodes.
         * @return the list of children nodes.
         */
        const ast_node_list_type& children() const noexcept {
            return m_children;
        }

        /**
         * Checks if this AST contains the given node.
         * @param descentant node to check if it is contained in this tree.
         * @return true if the given node is contained in this tree, false otherwise.
         */
        bool contains(const ast_node_ptr_type& descentant) const noexcept {
            assert(descentant);
            for (ast_node_ptr_type node = descentant; node; node = node->parent()) {
                if (node.get() == this) {
                    return true;
                }
            }
            return false;
        }

        /**
         * Adds a child node.
         * @param child node to add.
         */
        void add(const ast_node_ptr_type& child) noexcept {
            assert(child);
            assert(!child->parent());
            auto this_ptr = this->shared_from_this();
            assert(!child->contains(this_ptr));
            child->m_parent = this_ptr;
            child->m_it = m_children.insert(m_children.end(), child);
        }

        /**
         * Removes a child node.
         * @param child child node to remove.
         */
        void remove(const ast_node_ptr_type& child) noexcept {
            assert(child);
            assert(child->parent().get() == this);
            m_children.erase(child->m_it);
            child->m_parent.reset();
        }

        /**
         * Removes this node from its parent, if there is one.
         */
        void detach() {
            const auto p = parent();
            if (p) {
                p->remove(this->shared_from_this());
            }
        }

        /**
         * Returns the id of the node.
         * @return the id of the node.
         */
        const ast_node_id_type& id() const noexcept {
            return m_id;
        }

        /**
         * Sets the id of the node.
         * @param id the id of the node.
         */
        void set_id(const ast_node_id_type& id) noexcept {
            m_id = id;
        }

        /**
         * Returns the span of the node.
         * @return the span of the node.
         */
        const input_span_type& span() const noexcept {
            return m_span;
        }

        /**
         * Sets the span of the node.
         * @param span the span of the node.
         */
        void set_span(const input_span_type& span) noexcept {
            m_span = span;
        }

        /**
         * Returns the source that this node corresponds to.
         * If the input token is trivial, then it returns an std::basic_string_view,
         * otherwise it returns a vector that is copied from the source.
         * @return the source that this node corresponds to.
         */
        auto source() const noexcept {
            if constexpr (std::is_trivial_v<input_token_type>) {
                return std::basic_string_view<input_token_type>(m_span.data(), m_span.size());
            }
            else {
                return std::vector<input_token_type>(m_span.begin(), m_span.end());
            }
        }

        /**
         * Used for debugging, prints the AST in the given output stream.
         * @param stream output stream.
         * @param get_id_name function to convert the id of the node to a string.
         * @param depth tree depth; affects the idententation.
         * @param tabSize characters per tab.
         */
        template <class OutputStream, class GetIdName>
        void print(OutputStream& stream, const GetIdName& get_id_name, std::size_t depth = 0, std::size_t tabSize = 4) {
            for (std::size_t i = 0; i < depth * tabSize; ++i) {
                stream << ' ';
            }
            stream << get_id_name(m_id);
            if (m_children.empty()) {
                stream << " : " << source() << '\n';
            }
            else {
                stream << '\n';
                for (const ast_node_ptr_type& child : m_children) {
                    child->print(stream, get_id_name, depth + 1, tabSize);
                }
            }
        }

    private:
        using ast_node_weak_ptr = std::weak_ptr<ast_node>;
        using list_iterator_type = typename ast_node_list_type::iterator;
        
        ast_node_weak_ptr m_parent;
        list_iterator_type m_it;
        ast_node_list_type m_children;

        ast_node_id_type m_id;
        input_span_type m_span;
    };


    /**
     * AST node pointer type.
     * @param ParseDefinitions same as the ones in the ast_node class.
     */
    template <class ParseDefinitions>
    using ast_node_ptr_type = std::shared_ptr<ast_node<ParseDefinitions>>;


    /**
     * The default implementation of an AST node factory.
     * By default, it creates an instance of the given class.
     * Users of this library might create a custom factory which 
     * returns special node classes, depending on the match id.
     * @param ASTNode AST node class.
     */
    template <class ASTNode>
    class ast_node_factory {
    public:
        /**
         * It creates an instance of the given class, from the given match.
         * @param m match.
         * @return pointer to the created AST node.
         */
        template <class ParseDefinitions>
        ast_node_ptr_type<ParseDefinitions> operator ()(const match<ParseDefinitions>& m) const {
            return std::make_shared<ASTNode>(m.id(), m.span());
        }
    };


    /**
     * Creates an AST node, and its children, from a match and its children.
     * @param m match to create the AST node from.
     * @param ast_node_factory the AST node factory to use for creating AST nodes.
     * @return the created AST node.
     */
    template <class ParseDefinitions, class AstNodeFactory>
    ast_node_ptr_type<ParseDefinitions> create_ast_node(const match<ParseDefinitions>& m, const AstNodeFactory& ast_node_factory) {
        ast_node_ptr_type<ParseDefinitions> node = ast_node_factory(m);
        for (const match<ParseDefinitions>& childMatch : m.children()) {
            node->add(create_ast_node(childMatch, ast_node_factory));
        }
        return node;
    }

    /**
     * Creates an AST tree using the default ast node factory.
     * @param m the match to create the AST node for.
     * @return the created AST node.
     */
    template <class ParseDefinitions>
    ast_node_ptr_type<ParseDefinitions> create_ast_node(const match<ParseDefinitions>& m) {
        return create_ast_node(m, ast_node_factory<ast_node<ParseDefinitions>>());
    }


    /**
     * Creates an array of AST nodes from an array of matches.
     * @param ast_node_factory the AST node factory to use for creating AST nodes.
     * @param matches array of matches.
     * @return the created AST nodes.
     */
    template <class ParseDefinitions, class AstNodeFactory>
    std::vector<ast_node_ptr_type<ParseDefinitions>> create_ast_nodes(const std::vector<match<ParseDefinitions>>& matches, const AstNodeFactory& ast_node_factory) {
        std::vector<ast_node_ptr_type<ParseDefinitions>> ast_nodes;
        for (const match<ParseDefinitions>& match : matches) {
            ast_node_ptr_type<ParseDefinitions> node = create_ast_node(match, ast_node_factory);
            ast_nodes.push_back(node);
        }
        return ast_nodes;
    }


    /**
     * Creates an array of AST nodes from an array of matches using the default ast node factory.
     * @param matches array of matches.
     * @return the created AST nodes.
     */
    template <class ParseDefinitions>
    std::vector<ast_node_ptr_type<ParseDefinitions>> create_ast_nodes(const std::vector<match<ParseDefinitions>>& matches) {
        return create_ast_nodes(matches, ast_node_factory<ast_node<ParseDefinitions>>());
    }


} //namespace parserlib


#endif //PARSERLIB_AST_NODE_HPP
