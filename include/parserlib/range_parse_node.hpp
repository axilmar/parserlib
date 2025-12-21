#ifndef PARSERLIB_RANGE_PARSE_NODE_HPP
#define PARSERLIB_RANGE_PARSE_NODE_HPP


#include <cassert>
#include "parse_node.hpp"


namespace parserlib {


	/**
	 * Parses a symbol from within a range of symbols.
	 * @param Symbol the symbol type of the range.
	 */
	template <class Symbol>
	class range_parse_node : public parse_node<range_parse_node<Symbol>> {
	public:
		/**
		 * The constructor.
		 * @param min the minimum value of the range.
		 * @param max the maximum value of the range.
		 */
		range_parse_node(const Symbol& min, const Symbol& max)
			: m_min(min)
			, m_max(max)
		{
			assert(m_min <= m_max);
		}

		/**
		 * Parses a symbol from the range.
		 * @param pc the parse context.
		 * @return true if the current symbol is within the range, false otherwise.
		 */
		template <class ParseContext>
		bool parse(ParseContext& pc) const {
			if (pc.is_valid_parse_position()) {
				const auto current_symbol = pc.get_range();
				if (pc.compare_symbols(m_min, current_symbol) <= 0 && pc.compare_symbols(m_max, current_symbol) >= 0) {
					pc.increment_parse_position();
					return true;
				}
			}
			return false;
		}

	private:
		Symbol m_min;
		Symbol m_max;
	};


	/**
	 * Creates a range parse node.
 	 * @param min the minimum value of the range.
 	 * @param max the maximum value of the range.
	 * @return a range parse node.
	 */
	template <class Symbol>
	range_parse_node<Symbol> range(const Symbol& min, const Symbol& max) {
		return { min, max };
	}


} //namespace parserlib


#endif //PARSERLIB_RANGE_PARSE_NODE_HPP
