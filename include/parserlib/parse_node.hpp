#ifndef PARSERLIB_PARSE_NODE_HPP
#define PARSERLIB_PARSE_NODE_HPP


namespace parserlib {


    template <class ParseNode> class loop_0_parse_node;
    template <class ParseNode> class loop_1_parse_node;
    template <class ParseNode> class optional_parse_node;
    template <class ParseNode> class logical_and_parse_node;
    template <class ParseNode> class logical_not_parse_node;
    template <class ParseNode, class Annotation> class annotation_parse_node;


    class parse_node_base {
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
         * @param annotation the annotation object; if it is a nullptr_t instance,
         *  then an annotation is not added, and the parse node is instead returned,
         *  allowing optional cancellation of the annotation.
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
