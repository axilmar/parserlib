#ifndef PARSERLIB_AST_NODE_HPP
#define PARSERLIB_AST_NODE_HPP


#include <cassert>
#include <list>
#include <vector>
#include <memory>
#include "parse_definitions.hpp"
#include "match.hpp"


namespace parserlib {


    template <class ParseDefinitions = parse_definitions<>>
    class ast_node : public std::enable_shared_from_this<ast_node<ParseDefinitions>> {
    public:
        using input_iterator_type = typename ParseDefinitions::input_iterator_type;
        using input_token_type = typename ParseDefinitions::input_token_type;
        using input_span_type = span<input_iterator_type>;

        using ast_node_id_type = typename ParseDefinitions::output_token_type;
        using ast_node_ptr_type = std::shared_ptr<ast_node>;
        using ast_node_list_type = std::list<ast_node_ptr_type>;

        ast_node() noexcept {
        }

        ast_node(const ast_node_id_type& id, const input_span_type& span) noexcept
            : m_id(id)
            , m_span(span)
        {
        }

        virtual ~ast_node() {
        }

        ast_node_ptr_type parent() const noexcept {
            return m_parent.lock();
        }

        const ast_node_list_type& children() const noexcept {
            return m_children;
        }

        bool contains(const ast_node_ptr_type& descentant) const noexcept {
            assert(descentant);
            for (ast_node_ptr_type node = descentant; node; node = node->parent()) {
                if (node.get() == this) {
                    return true;
                }
            }
            return false;
        }

        void add(const ast_node_ptr_type& child) noexcept {
            assert(child);
            assert(!child->parent());
            auto this_ptr = this->shared_from_this();
            assert(!child->contains(this_ptr));
            child->m_parent = this_ptr;
            child->m_it = m_children.insert(m_children.end(), child);
        }

        void remove(const ast_node_ptr_type& child) noexcept {
            assert(child);
            assert(child->parent().get() == this);
            m_children.erase(child->m_it);
            child->m_parent.reset();
        }

        void detach() {
            const auto p = parent();
            if (p) {
                p->remove(this->shared_from_this());
            }
        }

        const ast_node_id_type& id() const noexcept {
            return m_id;
        }

        void set_id(const ast_node_id_type& id) noexcept {
            m_id = id;
        }

        const input_span_type& span() const noexcept {
            return m_span;
        }

        void set_span(const input_span_type& span) noexcept {
            m_span = span;
        }

        auto source() const noexcept {
            if constexpr (std::is_trivial_v<input_token_type>) {
                return std::basic_string_view<input_token_type>(m_span.data(), m_span.size());
            }
            else {
                return std::vector<input_token_type>(m_span.begin(), m_span.end());
            }
        }

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


    template <class ParseDefinitions>
    using ast_node_ptr_type = std::shared_ptr<ast_node<ParseDefinitions>>;


    class default_ast_node_factory {
    public:
        template <class ParseDefinitions>
        ast_node_ptr_type<ParseDefinitions> operator ()(const match<ParseDefinitions>& m) const {
            return std::make_shared<ast_node<ParseDefinitions>>(m.id(), m.span());
        }
    };


    template <class ParseDefinitions, class AstNodeFactory>
    ast_node_ptr_type<ParseDefinitions> create_ast_node(const match<ParseDefinitions>& m, const AstNodeFactory& astNodeFactory) {
        ast_node_ptr_type<ParseDefinitions> node = astNodeFactory(m);
        for (const match<ParseDefinitions>& childMatch : m.children()) {
            node->add(create_ast_node(childMatch, astNodeFactory));
        }
        return node;
    }

    template <class ParseDefinitions>
    ast_node_ptr_type<ParseDefinitions> create_ast_node(const match<ParseDefinitions>& m) {
        return create_ast_node(m, default_ast_node_factory());
    }

    template <class ParseDefinitions, class AstNodeFactory>
    std::vector<ast_node_ptr_type<ParseDefinitions>> create_ast_nodes(const std::vector<match<ParseDefinitions>>& matches, const AstNodeFactory& astNodeFactory) {
        std::vector<ast_node_ptr_type<ParseDefinitions>> ast_nodes;
        for (const match<ParseDefinitions>& match : matches) {
            ast_node_ptr_type<ParseDefinitions> node = create_ast_node(match, astNodeFactory);
            ast_nodes.push_back(node);
        }
        return ast_nodes;
    }


    template <class ParseDefinitions>
    std::vector<ast_node_ptr_type<ParseDefinitions>> create_ast_nodes(const std::vector<match<ParseDefinitions>>& matches) {
        return create_ast_nodes(matches, default_ast_node_factory());
    }


} //namespace parserlib


#endif //PARSERLIB_AST_NODE_HPP
