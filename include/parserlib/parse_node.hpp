#ifndef PARSERLIB_PARSE_NODE_HPP
#define PARSERLIB_PARSE_NODE_HPP


#include <type_traits>


namespace parserlib {


    /**
     * Base class for parse nodes.
     * It provides the unary operators for parse nodes.
     * @param Derived the type that was derived from this class.
     */
    template <class Derived> class parse_node {
    public:
        //ensure Derived is actually derived from this.
        static_assert(std::is_base_of_v<parse_node<Derived>, Derived>);

        /**
         * Returns pointer to derived class.
         * Used in cases a pointer to this is required,
         * since the operator & is used for the `logical AND` operation.
         * @return pointer to derived class.
         */
        const Derived* derived() const {
            return static_cast<const Derived*>(this);
        }
    };


    /**
     * Global function that can be overriden in order to convert a parse node
     * to another parse node type.
     * Used in globally defined operators which must convert their parameters
     * to parse nodes (for example, converting characters and strings to 
     * the relevant terminal parsers).
     * This default implementation simply returns the given parse node,
     * since it is already a parse node.
     * @param pn the parse node.
     * @return pn the given parse node.
     */
    template <class Derived> 
    const parse_node<Derived>& make_parse_node(const parse_node<Derived&> pn) {
        return pn;
    }


} //namespace parserlib


#endif //PARSERLIB_PARSE_NODE_HPP
