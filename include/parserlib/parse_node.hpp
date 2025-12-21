#ifndef PARSERLIB_PARSE_NODE_HPP
#define PARSERLIB_PARSE_NODE_HPP


namespace parserlib {


	template <class Child> class loop0_parse_node;
	template <class Child> class loop1_parse_node;
	template <class Child> class optional_parse_node;
	template <class Child> class logical_and_parse_node;
	template <class Child> class logical_not_parse_node;


	struct parse_node_tag {
	};


	/**
	 * Base class for parse nodes.
	 * It provides the unary operations for a parse node.
	 * @param Impl implementation type.
	 */
	template <class Impl>
	class parse_node : public parse_node_tag {
	public:
		/**
		 * Returns the implementation of the parse node. 
		 * @return the implementation of the parse node.
		 */
		const Impl* get_impl() const {
			return static_cast<const Impl*>(this);
		}

		/**
		 * Returns the implementation of the parse node.
		 * @return the implementation of the parse node.
		 */
		Impl* get_impl() {
			return static_cast<Impl*>(this);
		}

		/**
		 * Creates a 0-or-more times loop out for this parse node.
		 * @return a 0-or-more times loop parse node.
		 */
		loop0_parse_node<Impl> operator *() const {
			return *get_impl();
		}

		/**
		 * Creates a 1-or-more times loop out for this parse node.
		 * @return a 1-or-more times loop parse node.
		 */
		loop1_parse_node<Impl> operator +() const {
			return *get_impl();
		}

		/**
		 * Makes this parse node optional.
		 * @return an optional parse node.
		 */
		optional_parse_node<Impl> operator -() const {
			return *get_impl();
		}

		/**
		 * Makes this parse node a logical and predicate.
		 * @return a logical and parse node.
		 */
		logical_and_parse_node<Impl> operator &() const {
			return *get_impl();
		}

		/**
		 * Makes this parse node a logical not predicate.
		 * @return a logical not parse node.
		 */
		logical_not_parse_node<Impl> operator !() const {
			return *get_impl();
		}
	};


	/**
	 * Converts a parse node base type to its implementation type.
	 * @param pn the parse node to convert.
	 * @return the parse node's implementation type.
	 */
	template <class Impl>
	const Impl& make_parse_node(const parse_node<Impl>& pn) {
		return *pn.get_impl();
	}


} //namespace parserlib


#endif //PARSERLIB_PARSE_NODE_HPP
