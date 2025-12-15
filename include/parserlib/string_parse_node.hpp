#ifndef PARSERLIB_STRING_PARSE_NODE_HPP
#define PARSERLIB_STRING_PARSE_NODE_HPP


#include <string_view>
#include <vector>
#include "parse_node.hpp"


namespace parserlib {


    /**
     * A parse node class that parses a string.
     * @param Char type of character; it must be convertible to 'int'.
     */
    template <class Char>
    class string_parse_node : public parse_node<string_parse_node<Char>> {
    public:
        /**
         * The constructor.
         * @param string the string to parse.
         */
        string_parse_node(const std::basic_string_view<Char>& string)
            : m_string(string)
            , m_symbol_sequence(get_symbol_sequence(string))
        {
        }

        /**
         * Parses a string.
         * @param pc the context to use for parsing.
         * @return true on success, false on failure.
         */
        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            return pc.parse_symbol_sequence(m_symbol_sequence);
        }

    private:
        std::basic_string_view<Char> m_string;
        std::vector<int> m_symbol_sequence;

        static std::vector<int> get_symbol_sequence(const std::basic_string_view<Char>& string) {
            std::vector<int> result;
            for (const Char& ch : string) {
                result.push_back(static_cast<int>(ch));
            }
            return result;
        }
    };


    /**
     * Creates a parse node for a string.
     * @param string string to create a string parse node of.
     * @return a string parse node.
     */
    template <class Char>
    string_parse_node<Char> terminal(const Char* string) {
        return std::basic_string_view<Char>(string);
    }


    /**
     * Creates a parse node for a string.
     * @param string string to create a string parse node of.
     * @return a string parse node.
     */
    template <class Char>
    string_parse_node<Char> make_parse_node(const Char* string) {
        return std::basic_string_view<Char>(string);
    }


} //namespace parserlib


#endif //PARSERLIB_STRING_PARSE_NODE_HPP
