#ifndef PARSERLIB_AST_HPP
#define PARSERLIB_AST_HPP


#include <memory>
#include <vector>
#include <algorithm>
#include "parse_context_data.hpp"
#include "parse_context_default_options.hpp"


namespace parserlib {


    /**
     * Base class for an Abstract Syntax Tree (AST) node.
     * @param Source source type.
     * @param MatchId match id type.
     * @param TextPosition text position type.
     */
    template <class Source = std::string, class MatchId = int, class TextPosition = default_text_position>
    class ast_node 
        : public source_partition<Source, MatchId, TextPosition>
        , public std::enable_shared_from_this<ast_node<Source, MatchId, TextPosition>>
    {
    public:
        /** Source type. */
        using source_type = Source;

        /** Iterator type. */
        using iterator_type = typename Source::const_iterator;

        /** Match id type. */
        using match_id_type = MatchId;

        /** Text position type. */
        using text_position_type = TextPosition;

        /** Parse position type. */
        using parse_position_type = parse_position<Source, TextPosition>;

        /** Source partition type. */
        using source_partition_type = source_partition<Source, MatchId, TextPosition>;

        /** AST node ptr type. */
        using ast_node_ptr_type = std::shared_ptr<ast_node>;

        /** AST node ptr container type. */
        using ast_node_container_type = std::vector<ast_node_ptr_type>;

        /**
         * The constructor.
         * @param id the id of the node.
         * @param start_pos start position of the range within the source that this node represents.
         * @param end_iterator end iterator of the range within the source.
         */
        ast_node(const match_id_type& id, const parse_position_type& start_pos, const iterator_type& end_iterator)
            : source_partition_type(id, start_pos, end_iterator)
        {
        }

        /**
         * The destructor.
         * Virtual due to possible polymorphism.
         */
        virtual ~ast_node() {
        }

        /**
         * Returns the parent node.
         * @return the parent node; null ptr if there is no parent.
         */
        ast_node_ptr_type parent() const {
            return m_parent ? m_parent->shared_from_this() : nullptr;
        }

        /**
         * Returns the children.
         * @return the children.
         */
        const ast_node_container_type& children() const {
            return m_children;
        }

        /**
         * Adds a child node.
         * If the child belongs to another node, then it is removed from that node before added to this node.
         * @param child the child node.
         * @param index position to insert the node; if -1, then it is added to the end of the children's container.
         */
        void add_child(const ast_node_ptr_type& child, size_t index = static_cast<size_t>(-1)) {
            if (child->m_parent) {
                child->m_parent->remove_child(child->shared_from_this());
            }
            m_children.insert(std::next(m_children.begin(), std::min(m_children.size(), index)), child);
            child->m_parent = this;
        }

        /**
         * Removes a child node.
         * @param child child node to remove.
         * @exception std::invalid_argument if the given child is not a child of this node.
         */
        void remove_child(const ast_node_ptr_type& child) {
            if (child->m_parent != this) {
                throw std::invalid_argument("child");
            }
            child->m_parent = nullptr;
            m_children.erase(std::find(m_children.begin(), m_children.end(), child));
        }

        /**
         * Removes all children.
         */
        void remove_children() {
            for (const ast_node_ptr_type& child : m_children) {
                child->m_parent = nullptr;
            }
            m_children.clear();
        }

    private:
        ast_node* m_parent{nullptr};
        ast_node_container_type m_children;
    };


    /**
     * The default AST factory.
     * If AST node subclassing is required, a specific factory class shall be created
     * which creates AST node subclasses based on the id of the given match.
     * @param Source source type.
     * @param MatchId match id type.
     * @param TextPosition text position type.
     */
    template <class Source, class MatchId, class TextPosition>
    class default_ast_factory {
    public:
        /**
         * Allocates a shared instance of ast_node.
         * @param m match to use for allocating a shared ast node instance.
         * @return shared pointer to the allocated ast node instance.
         */
        static std::shared_ptr<ast_node<Source, MatchId, TextPosition>> 
            make_node(const match<Source, MatchId, TextPosition>& m)
        {
            return std::make_shared<ast_node<Source, MatchId, TextPosition>>(m.id(), m.start_position(), m.end_iterator());
        }
    };


    /**
     * Creates an AST from the given match and its children.
     * @param m match to create an AST node for.
     * @param factory the AST node factory.
     * @return shared pointer to the allocated ast node instance.
     */
    template <class Source, class MatchId, class TextPosition, class Factory>
    std::shared_ptr<ast_node<Source, MatchId, TextPosition>> 
        make_ast(const match<Source, MatchId, TextPosition>& m, const Factory& factory)
    {
        std::shared_ptr<ast_node<Source, MatchId, TextPosition>> node = factory.make_node(m);
        for (const auto& child_match : m.children()) {
            std::shared_ptr<ast_node<Source, MatchId, TextPosition>> child = make_ast(child_match, factory);
            node->add_child(child);
        }
        return node;
    }


    /**
     * Creates an AST from the given match and its children, using the default AST factory.
     * @param m match to create an AST node for.
     * @return shared pointer to the allocated ast node instance.
     */
    template <class Source, class MatchId, class TextPosition>
    std::shared_ptr<ast_node<Source, MatchId, TextPosition>>
        make_ast(const match<Source, MatchId, TextPosition>& m)
    {
        return make_ast(m, default_ast_factory<Source, MatchId, TextPosition>());
    }


} //namespace parserlib


#endif //PARSERLIB_AST_HPP
