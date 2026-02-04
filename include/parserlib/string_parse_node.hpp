#ifndef PARSERLIB_STRING_PARSE_NODE_HPP
#define PARSERLIB_STRING_PARSE_NODE_HPP


#include <string_view>
#include "parse_node.hpp"


namespace parserlib {


	/**
	 * A string parse node.
	 * @param Char character type.
	 * @param CharTraits character traits.
	 */
	template <class Char, class CharTraits>
	class string_parse_node : public parse_node<string_parse_node<Char, CharTraits>> {
	public:
		/**
		 * The constructor.
		 * @param string the string to parse.
		 */
		string_parse_node(const std::basic_string_view<Char, CharTraits>& string)
			: m_string(string)
		{
		}

		/**
		 * Parses the string.
		 * @param pc the parse context.
		 * @return true if the string matches the source, false otherwise.
		 */
		template <class ParseContext>
		bool parse(ParseContext& pc) const {
			if (pc.is_valid_parse_position()) {
				auto itString = m_string.begin();
				auto itSource = pc.get_iterator();
				for (;;) {
					if (itString == m_string.end()) {
						pc.increment_parse_position(m_string.size());
						return true;
					}
					if (itSource == pc.get_state().get_end_iterator()) {
						break;
					}
					const auto string_char = *itString;
					const auto source_char = *itSource;
					if (pc.compare_symbols(string_char, source_char) != 0) {
						break;
					}
					++itString;
					++itSource;
				}
			}
			return false;
		}

	private:
		std::basic_string_view<Char, CharTraits> m_string;
	};


	/**
	 * Creates a string parse node.
	 * @param string the string to create a parse node for.
	 * @return a string parse node.
	 */
	template <class Char>
	string_parse_node<Char, std::char_traits<Char>> terminal(const Char* string) {
		return std::basic_string_view<Char, std::char_traits<Char>>(string);
	}


	/**
	 * Converts a string to a parse node.
	 * @param string the string to create a parse node for.
	 * @return a string parse node.
	 */
	template <class Char>
	string_parse_node<Char, std::char_traits<Char>> make_parse_node(const Char* string) {
		return std::basic_string_view<Char, std::char_traits<Char>>(string);
	}


	/**
	 * Creates a symbol parse node for char string value.
	 * @param symbol the symbol to create a parse node for.
	 * @return a symbol parse node.
	 */
	inline string_parse_node<char, std::char_traits<char>> operator ""_term(const char* string, size_t size) {
		return std::basic_string_view<char, std::char_traits<char>>(string);
	}    


	/**
	 * Creates a symbol parse node for wchar_t string value.
	 * @param symbol the symbol to create a parse node for.
	 * @return a symbol parse node.
	 */
	inline string_parse_node<wchar_t, std::char_traits<wchar_t>> operator ""_term(const wchar_t* string, size_t size) {
		return std::basic_string_view<wchar_t, std::char_traits<wchar_t>>(string);
	}    


} //namespace parserlib


#endif //PARSERLIB_STRING_PARSE_NODE_HPP
