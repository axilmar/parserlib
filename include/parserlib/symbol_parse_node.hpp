#ifndef PARSERLIB_SYMBOL_PARSE_NODE_HPP
#define PARSERLIB_SYMBOL_PARSE_NODE_HPP


#include <type_traits>
#include "parse_node.hpp"


namespace parserlib {


	/**
	 * Parses a single symbol.
	 * @param Symbol the symbol type to parse.
	 */
	template <class Symbol>
	class symbol_parse_node : public parse_node<symbol_parse_node<Symbol>> {
	public:
		/**
		 * The constructor.
		 * @param sybmol the symbol to parse.
		 */
		symbol_parse_node(const Symbol& symbol)
			: m_symbol(symbol)
		{
		}

		/**
		 * Parses the symbol.
		 * @param pc the parse context.
		 * @return true if the symbol matches the current symbol in the source, false otherwise.
		 */
		template <class ParseContext>
		bool parse(ParseContext& pc) const {
			if (pc.is_valid_parse_position()) {
				const auto current_symbol = pc.get_symbol();
				if (pc.compare_symbols(m_symbol, current_symbol) == 0) {
					pc.increment_parse_position();
					return true;
				}
			}
			return false;
		}

	private:
		Symbol m_symbol;
	};


	/**
	 * Creates a symbol parse node.
	 * @param symbol the symbol to create a parse node for.
	 * @return a symbol parse node.
	 */
	template <class Symbol>
	symbol_parse_node<Symbol> terminal(const Symbol& symbol) {
		return symbol;
	}


	/**
	 * Converts a value to a symbol parse node.
	 * The value must be a non-parse node and not a boolean.
	 * @param symbol the symbol to create a parse node for.
	 * @return a symbol parse node.
	 */
	template <class Symbol, std::enable_if_t<!std::is_base_of_v<parse_node_tag, Symbol> && !std::is_same_v<bool, Symbol>, bool> = true>
	symbol_parse_node<Symbol> make_parse_node(const Symbol& symbol) {
		return symbol;
	}


} //namespace parserlib


#endif //PARSERLIB_SYMBOL_PARSE_NODE_HPP
