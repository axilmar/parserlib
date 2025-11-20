#ifndef PARSERLIB_PARSE_NODE_HPP
#define PARSERLIB_PARSE_NODE_HPP


#include <string>
#include <type_traits>
#include <mutex>
#include "id_name.hpp"


namespace parserlib {


    template <class ParseNode> class loop_0_parse_node;
    template <class ParseNode> class loop_1_parse_node;
    template <class ParseNode> class optional_parse_node;
    template <class ParseNode> class logical_and_parse_node;
    template <class ParseNode> class logical_not_parse_node;
    template <class ParseNode, class Annotation> class annotation_parse_node;

    /**
     * Base class for parse nodes.
     */
    class parse_node_base {
    public:
        /**
         * Virtual destructor due to polymorphism.
         */
        virtual ~parse_node_base() {
        }

        #ifndef NDEBUG
        /**
         * Provided so as that subclasses can initialize the tree of nodes before any parse.
         * The default implementation is empty.
         */
        virtual void init_tree() const {
        }
        #endif

        #ifndef NDEBUG
        /**
         * Does dynamic initialization of the parse node.
         * It debug mode, it sets the `m_text` member of the parse node, if not set yet.
         * Setting the text dynamically is required due to rules being recursive.
         * The operation is thread-safe (protected by a recursive mutex, avoiding recursive rules infinite recursion).
         * In release mode, it does nothing.
         * This function will be invoked automatically from `parse_context::parse(parse_node)`.
         */
        void init() const {
            if (m_mutex.try_lock()) {
                if (m_text.empty()) {
                    m_text = text();
                    init_tree();
                }
                m_mutex.unlock();
            }
        }
        #endif

        /**
         * Interface for returning the text description of the node.
         * @return the text description of the node.
         */
        virtual std::string text() const = 0;

    protected:
        /**
         * The default constructor.
         */
        parse_node_base() {
        }

        #ifndef NDEBUG
        /**
         * The copy constructor.
         * @param src the source object.
         */
        parse_node_base(const parse_node_base& src)
            : m_text(src.m_text)
        {
        }
        #endif

        #ifndef NDEBUG
        /**
         * The copy assignment operator.
         * @param src the source object.
         * @return reference to this.
         */
        parse_node_base& operator = (const parse_node_base& src) {
            m_text = src.m_text;
            return *this;
        }
        #endif

    private:
        #ifndef NDEBUG
        mutable std::string m_text;
        mutable std::recursive_mutex m_mutex;
        #endif
    };


    /**
     * Base class for parse nodes.
     * It provides the unary operators for parse nodes.
     * @param Derived the type that was derived from this class.
     */
    template <class Derived> class parse_node : public parse_node_base {
    public:
        /**
         * Returns pointer to derived class.
         * Used in cases a pointer to this is required,
         * since the operator & is used for the `logical AND` operation.
         * @return pointer to derived class.
         */
        const Derived* derived() const {
            return static_cast<const Derived*>(this);
        }

        /**
         * Converts this parse node into a loop which is repeated 0 or more times.
         * @return a loop which is repeated 0 or more times.
         */
        loop_0_parse_node<Derived> operator *() const;

        /**
         * Converts this parse node into a loop which is repeated 1 or more times.
         * @return a loop which is repeated 1 or more times.
         */
        loop_1_parse_node<Derived> operator +() const;

        /**
         * Converts this parse node into an optional parse node.
         * @return an optional parse node.
         */
        optional_parse_node<Derived> operator -() const;

        /**
         * Converts this parse node into a logical AND parse node.
         * @return a logical AND parse node.
         */
        logical_and_parse_node<Derived> operator &() const;

        /**
         * Converts this parse node into a logical NOT parse node.
         * @return a logical NOT parse node.
         */
        logical_not_parse_node<Derived> operator !() const;

        /**
         * Returns a node that adds an annotation to another node.
         * @param annotation the annotation object.
         * @return an annotation parse node.
         */
        template <class Annotation>
        auto operator [](const Annotation& annotation) const {
            return make_annotation_parse_node(*this->derived(), annotation);
        }
    };


    /**
     * Global function that can be overriden in order to convert a value
     * to a parse node type.
     * Used in globally defined operators which must convert their parameters
     * to parse nodes (for example, converting characters and strings to 
     * the relevant terminal parsers).
     * This default implementation returns the derived type instance of the given parse node.
     * @param pn the parse node.
     * @return the derived type instance of the given parse node.
     */
    template <class Derived> 
    auto make_parse_node(const parse_node<Derived>& pn) {
        return *pn.derived();
    }


} //namespace parserlib


#endif //PARSERLIB_PARSE_NODE_HPP
