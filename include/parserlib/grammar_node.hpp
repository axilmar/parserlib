#ifndef PARSERLIB_GRAMMAR_NODE_HPP
#define PARSERLIB_GRAMMAR_NODE_HPP


#include <type_traits>
#include <functional>
#include "vector.hpp"
#include "symbol_comparator.hpp"
#include "parse_context_base.hpp"


namespace parserlib {


    class rule;


    /**
     * Grammar node type enumeration.
     */ 
    enum grammar_node_type {
        grammar_node_type_ref,
        grammar_node_type_symbol,
        grammar_node_type_string,
        grammar_node_type_set,
        grammar_node_type_range,
        grammar_node_type_loop_0,
        grammar_node_type_loop_1,
        grammar_node_type_optional,
        grammar_node_type_logical_and,
        grammar_node_type_logical_not,
        grammar_node_type_sequence,
        grammar_node_type_choice,
        grammar_node_type_match,
        grammar_node_type_any,
        grammar_node_type_end,
        grammar_node_type_false,
        grammar_node_type_true,
        grammar_node_type_function,
        grammar_node_type_custom
    };


    /**
     * Base class for grammar nodes.
     */
    class grammar_node {
    public:
        /**
         * Virtual destructor due to polymorphism.
         */ 
        virtual ~grammar_node() {
        }

        /**
         * Returns the node's type.
         * @return the node's type.
         */ 
        grammar_node_type get_type() const {
            return m_type;
        }

    protected:
        /**
         * The constructor.
         * It can only be instantiated through subclasses.
         * @param type the node type.
         */ 
        grammar_node(grammar_node_type type) : m_type(type) {}

    private:
        grammar_node_type m_type;
    };


    /**
     * Pointer to grammar node.
     * It contains the unary operator methods.
     */
    class grammar_node_ptr : public std::shared_ptr<grammar_node> {
    public:
        using std::shared_ptr<grammar_node>::shared_ptr;

        /**
         * Constructor from shared ptr to node.
         * @param node the source node.
         */ 
        grammar_node_ptr(const std::shared_ptr<grammar_node>& node)
            : std::shared_ptr<grammar_node>(node)
        {
        }

        /**
         * Creation of symbol grammar node for a symbol.
         * @param symbol the symbol to create a node for.
         */ 
        template <class T>
        grammar_node_ptr(const T& symbol);

        /**
         * Creation of symbol string grammar node for a string.
         * @param symbol the symbol to create a node for.
         */ 
        template <class T>
        grammar_node_ptr(const T* symbol);

        /**
         * Creation of symbol grammar node for a rule.
         * @param r the rule to create a node for.
         */ 
        inline grammar_node_ptr(const rule& r);

        /**
         * Creates a zero-or-more times loop for this grammar node.
         * @return a zero-or-more times loop for this grammar node.
         */  
        inline grammar_node_ptr operator *() const;

        /**
         * Creates a one-or-more times loop for this grammar node.
         * @return a one-or-more times loop for this grammar node.
         */  
        inline grammar_node_ptr operator +() const;

        /**
         * Makes this node optional.
         * @return an optional node for this node.
         */  
        inline grammar_node_ptr operator -() const;

        /**
         * Makes this node a logical and predicate.
         * @return a logical and node for this node.
         */  
        inline grammar_node_ptr operator &() const;

        /**
         * Makes this node a logical not predicate.
         * @return a logical not node for this node.
         */  
        inline grammar_node_ptr operator !() const;

        /**
         * Helper function for parsing.
         * @param pc the parse context to use.
         * @return true on success, false on error.
         */
        template <class ParseContext, class SymbolComparator = default_symbol_comparator>
        inline bool parse(ParseContext& pc) const;
    };


    /**
     * Type of grammar node container.
     */ 
    using grammar_node_container = std::vector<grammar_node_ptr>;


    /**
     * Base class for grammar nodes with one child.
     */
    class parent_grammar_node_single_child : public grammar_node {
    public:
        /**
         * Returns the child.
         * @return the child.
         */ 
        const grammar_node_ptr& get_child() const {
            return m_child;
        }

    protected:
        /**
         * The constructor.
         * It can only be instantiated through subclasses.
         * @param type the node type.
         * @param child the child child node; optional.
         */ 
        parent_grammar_node_single_child(grammar_node_type type, const grammar_node_ptr& child = {})
            : grammar_node(type)
            , m_child(child)
        {
        }

    private:
        grammar_node_ptr m_child;
    };


    /**
     * Base class for grammar nodes with multiple children.
     */
    class parent_grammar_node : public grammar_node {
    public:
        /**
         * Returns the children.
         * @return the children.
         */ 
        const grammar_node_container& get_children() const {
            return m_children;
        }

    protected:
        /**
         * The constructor.
         * It can only be instantiated through subclasses.
         * @param type the node type.
         * @param child the child child node; optional.
         */ 
        parent_grammar_node(grammar_node_type type, grammar_node_container&& children = {})
            : grammar_node(type)
            , m_children(std::move(children))
        {
        }

    private:
        grammar_node_container m_children;
    };


    /**
     * A grammar node that can be used to refer to another grammar node.
     * Internally, it uses a weak pointer, so as that referential circles
     * are not created.
     */ 
    class ref_grammar_node : public grammar_node {
    public:
        ref_grammar_node(const grammar_node_ptr& node = {}) 
            : grammar_node(grammar_node_type_ref) 
            , m_node(node)
        {
        }

        /**
         * Returns the currently referenced node.
         */ 
        const grammar_node_ptr get_node() const {
            return m_node.lock();
        }

        /**
         * Sets the node to refer to.
         * @param node the ndoe to refer to.
         */ 
        void set_node(const grammar_node_ptr& node) {
            m_node = node;
        }

    private:
        std::weak_ptr<grammar_node> m_node;
    };


    /**
     * Pointer to a `ref_grammar_node`.
     */ 
    using ref_grammar_node_ptr = std::shared_ptr<ref_grammar_node>;


    /**
     * A grammar node for parsing a single symbol.
     */
    class symbol_grammar_node : public grammar_node {
    public:
        /**
         * The constructor.
         * @param value the symbol value.
         */ 
        template <class T>
        symbol_grammar_node(const T& value) 
            : grammar_node(grammar_node_type_symbol)
            , m_symbol(std::make_unique<typed_symbol<T>>(value))
        {
        }

        /**
         * Returns the symbol.
         * @return the symbol.
         */ 
        const symbol_ptr& get_symbol() const {
            return m_symbol;
        }

    private:
        symbol_ptr m_symbol;
    };


    /**
     * A grammar node for parsing a string.
     */
    class string_grammar_node : public grammar_node {
    public:
        /**
         * Constructor from string view.
         * @param value the symbol value.
         */ 
        template <class T>
        string_grammar_node(const std::basic_string_view<T>& value) 
            : grammar_node(grammar_node_type_string)
            , m_symbol(std::make_unique<typed_symbol_string<T>>(value))
        {
        }

        /**
         * Constructor from null-terminated string.
         * @param value the symbol value.
         */ 
        template <class T>
        string_grammar_node(const T* value) 
            : string_grammar_node(std::basic_string_view<T>(value))
        {
        }

        /**
         * Returns the symbol.
         * @return the symbol.
         */ 
        const symbol_string_ptr& get_symbol() const {
            return m_symbol;
        }

    private:
        symbol_string_ptr m_symbol;
    };


    /**
     * A grammar node for parsing a symbol out of a set.
     */
    class set_grammar_node : public grammar_node {
    public:
        /**
         * The constructor.
         * @param symbol the symbol set.
         */
        template <class T>
        set_grammar_node(std::unique_ptr<typed_symbol_set<T>>&& symbol) 
            : grammar_node(grammar_node_type_set)
            , m_symbol(std::move(symbol))
        {
        }

        /**
         * Constructor from a set of values from a string.
         * @param value the symbol value.
         */
        template <class T>
        set_grammar_node(const std::basic_string_view<T>& value) 
            : set_grammar_node(std::make_unique<typed_symbol_set<T>>(value))
        {
        }

        /**
         * Constructor from a null-terminated string.
         * @param value the symbol value.
         */
        template <class T>
        set_grammar_node(const T* value) 
            : set_grammar_node(std::basic_string_view<T>(value))
        {
        }

        /**
         * Constructor from a set of values from an initializer list.
         * @param value the symbol value.
         */
        template <class T>
        set_grammar_node(const std::initializer_list<T>& value) 
            : set_grammar_node(std::make_unique<typed_symbol_set<T>>(value))
        {
        }

        /**
         * Constructor from a set of values from a vector.
         * @param value the symbol value.
         */
        template <class T>
        set_grammar_node(const std::vector<T>& value) 
            : set_grammar_node(std::make_unique<typed_symbol_set<T>>(value))
        {
        }

        /**
         * Returns the symbol.
         * @return the symbol.
         */ 
        const symbol_set_ptr& get_symbol() const {
            return m_symbol;
        }

    private:
        symbol_set_ptr m_symbol;
    };


    /**
     * A grammar node for parsing a symbol out of a range.
     */
    class range_grammar_node : public grammar_node {
    public:
        /**
         * The constructor.
         * @param min the min value.
         * @param max the max value.
         */ 
        template <class T>
        range_grammar_node(const T& min, const T& max) 
            : grammar_node(grammar_node_type_range)
            , m_symbol(std::make_unique<typed_symbol_pair<T, T>>(min, max))
        {
            assert(min < max);
        }

        /**
         * Returns the symbol.
         * @return the symbol.
         */ 
        const symbol_pair_ptr& get_symbol() const {
            return m_symbol;
        }

    private:
        symbol_pair_ptr m_symbol;
    };


    /**
     * A grammar node used for parsing another grammar node 0 or more times.
     */ 
    class loop_0_grammar_node : public parent_grammar_node_single_child {
    public:
        /**
         * The constructor.
         * @param child the child child node; optional.
         */ 
        loop_0_grammar_node(const grammar_node_ptr& child = {})
            : parent_grammar_node_single_child(grammar_node_type_loop_0, child)
        {
        }
    };


    /**
     * A grammar node used for parsing another grammar node 1 or more times.
     */ 
    class loop_1_grammar_node : public parent_grammar_node_single_child {
    public:
        /**
         * The constructor.
         * @param child the child child node; optional.
         */ 
        loop_1_grammar_node(const grammar_node_ptr& child = {})
            : parent_grammar_node_single_child(grammar_node_type_loop_1, child)
        {
        }
    };


    /**
     * A grammar node used for parsing another grammar node as an optional node.
     * If optional node parsing fails, the parse context is restored to its previous state.
     */ 
    class optional_grammar_node : public parent_grammar_node_single_child {
    public:
        /**
         * The constructor.
         * @param child the child child node; optional.
         */ 
        optional_grammar_node(const grammar_node_ptr& child = {})
            : parent_grammar_node_single_child(grammar_node_type_optional, child)
        {
        }
    };


    /**
     * A grammar node that allows another grammar node to be used as a positive test.
     * No state is allowed to change during using a grammar node as a positive test.
     */ 
    class logical_and_grammar_node : public parent_grammar_node_single_child {
    public:
        /**
         * The constructor.
         * @param child the child child node; optional.
         */ 
        logical_and_grammar_node(const grammar_node_ptr& child = {})
            : parent_grammar_node_single_child(grammar_node_type_logical_and, child)
        {
        }
    };


    /**
     * A grammar node that allows another grammar node to be used as a negative test.
     * No state is allowed to change during using a grammar node as a negative test.
     */ 
    class logical_not_grammar_node : public parent_grammar_node_single_child {
    public:
        /**
         * The constructor.
         * @param child the child child node; optional.
         */ 
        logical_not_grammar_node(const grammar_node_ptr& child = {})
            : parent_grammar_node_single_child(grammar_node_type_logical_not, child)
        {
        }
    };


    /**
     * A grammar node that uses a list of grammar nodes for parsing.
     * If at least one grammar node fails to be parsed, then the whole sequence is invalidated
     * and the parse context state is restored to the state before the sequence.
     * Sequences are flattened, i.e. a sequence of sequences becomes a single sequence.
     */ 
    class sequence_grammar_node : public parent_grammar_node {
    public:
        sequence_grammar_node(grammar_node_container&& children) : 
            parent_grammar_node(grammar_node_type_sequence, std::move(children))
        {
        }
    };


    /**
     * A grammar node that uses a list of grammar nodes for parsing.
     * At least one child must succeed.
     * Children are tested in the order they are declared in the grammar.
     * Choices are flattened, i.e. a choice of choices becomes a single choice.
     */ 
    class choice_grammar_node : public parent_grammar_node {
    public:
        choice_grammar_node(grammar_node_container&& children) 
            : parent_grammar_node(grammar_node_type_choice, std::move(children))
        {
        }
    };


    /**
     * A grammar node that represents a match.
     */
    class match_grammar_node : public parent_grammar_node_single_child {
    public:
        /**
         * The constructor.
         * @param child the child node to use for parsing.
         * @param id the id to use for this match.
         */
        template <class T>
        match_grammar_node(const grammar_node_ptr& child, const T& id)
            : parent_grammar_node_single_child(grammar_node_type_match, child)
            , m_id_symbol(std::make_unique<typed_symbol<T>>(id))
        {
        }

        /**
         * Returns the match id symbol.
         * @return the match id symbol.
         */ 
        const symbol_ptr& get_id_symbol() const {
            return m_id_symbol;
        }

    private:
        symbol_ptr m_id_symbol;
    };


    /**
     * A grammar node that can be used to parse any symbol.
     */ 
    class any_grammar_node : public grammar_node {
    public:
        any_grammar_node() : grammar_node(grammar_node_type_any) {}
    };


    /**
     * A grammar node that can be used to test if the source end is reached.
     */ 
    class end_grammar_node : public grammar_node {
    public:
        end_grammar_node() : grammar_node(grammar_node_type_end) {}
    };


    /**
     * A grammar node that can be used to invalidate parsing.
     */ 
    class false_grammar_node : public grammar_node {
    public:
        false_grammar_node() : grammar_node(grammar_node_type_false) {}
    };


    /**
     * A grammar node that can be used to validate parsing.
     */ 
    class true_grammar_node : public grammar_node {
    public:
        true_grammar_node() : grammar_node(grammar_node_type_true) {}
    };


    /**
     * Parse function.
     * @param pc the parse context.
     * @return true on success, false on failure.
     */ 
    using parse_function = std::function<bool(parse_context_base& pc)>;


    /**
     * A grammar node that holds a function to be used for parsing.
     */ 
    class function_grammar_node : public grammar_node {
    public:
        /**
         * The constructor.
         * @param f the function.
         */ 
        template <class T, std::enable_if_t<std::is_base_of_v<parse_context_base, T>, bool> = true>
        function_grammar_node(const std::function<bool(T&)>& f)
            : grammar_node(grammar_node_type_function)
            , m_function([=](parse_context_base& pc) { return f(dynamic_cast<T&>(pc)); })
        {
        }

        /**
         * Returns the function.
         * @return the function.
         */ 
        const parse_function& get_function() const {
            return m_function;
        }

    private:
        parse_function m_function;
    };


    template <class T>
    grammar_node_ptr::grammar_node_ptr(const T& symbol)
        : std::shared_ptr<grammar_node>(std::make_shared<symbol_grammar_node>(symbol))
    {
    }


    template <class T>
    grammar_node_ptr::grammar_node_ptr(const T* symbol)
        : std::shared_ptr<grammar_node>(std::make_shared<string_grammar_node>(symbol))
    {
    }


} //namespace parserlib


#endif //PARSERLIB_GRAMMAR_NODE_HPP
