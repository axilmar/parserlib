#ifndef PARSERLIB_SET_PARSE_NODE_HPP
#define PARSERLIB_SET_PARSE_NODE_HPP


#include <vector>
#include <string_view>
#include "parse_node.hpp"


namespace parserlib {


	/**
	 * A set parse node.
	 * @param Symbol symbol type.
	 */
	template <class Symbol>
	class set_parse_node : public parse_node<set_parse_node<Symbol>> {
	public:
		/**
		 * The constructor.
		 * @param set the set to parse.
		 */
		set_parse_node(const std::vector<Symbol>& set)
			: m_set(set)
		{
		}

		/**
		 * Parses the set.
		 * @param pc the parse context.
		 * @return true if the set matches the source, false otherwise.
		 */
		template <class ParseContext>
		bool parse(ParseContext& pc) const {
			if (pc.is_valid_parse_position()) {
				const auto current_symbol = pc.get_symbol();
				for (const Symbol& symbol : m_set) {
					if (pc.compare_symbols(symbol, current_symbol) == 0) {
						pc.increment_parse_position();
						return true;
					}
				}
			}
			return false;
		}

	private:
		std::vector<Symbol> m_set;
	};


	/**
	 * Creates a set parse node.
	 * @param set the set to create a parse node for.
	 * @return a set parse node.
	 */
	template <class Symbol>
	set_parse_node<Symbol> set(const Symbol* set) {
		std::basic_string_view<Symbol> set_string(set);
		return std::vector<Symbol>(set_string.begin(), set_string.end());
	}


} //namespace parserlib


#endif //PARSERLIB_SET_PARSE_NODE_HPP
