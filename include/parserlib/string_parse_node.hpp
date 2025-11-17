#ifndef PARSERLIB_STRING_PARSE_NODE_HPP
#define PARSERLIB_STRING_PARSE_NODE_HPP


#include <type_traits>
#include <string>
#include "parse_node.hpp"


namespace parserlib {


    /**
     * A string parse node.
     * @param Symbol string element type.
     */
    template <class Symbol>
    class string_parse_node : public parse_node<string_parse_node<Symbol>> {
    public:
        /**
         * The constructor.
         * @param string the string to parse.
         */
        string_parse_node(const std::basic_string<Symbol>& string)
            : m_string(string)
            #ifndef NDEBUG
            , m_text('"' + m_string + '"')
            #endif
        {
        }

        /**
         * Parses the string against the current input.
         * @param pc the current parse context.
         * @return true if the whole string is parsed successfully, false otherwise.
         */
        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            if (pc.parse_valid() && pc.terminal_parsing_allowed()) {
                auto thisIt = m_string.begin();
                auto parseIt = pc.parse_position().iterator();
                for (;;) {
                    if (thisIt == m_string.end()) {
                        pc.increment_parse_position(m_string.size());
                        return true;
                    }
                    if (parseIt == pc.end_iterator()) {
                        break;
                    }
                    if (pc.compare_symbols(*thisIt, *parseIt) != 0) {
                        break;
                    }
                    ++thisIt;
                    ++parseIt;
                }
            }
            return false;
        }

        #ifndef NDEBUG
        const std::string& text() const {
            return m_text;
        }
        #endif

    private:
        const std::basic_string<Symbol> m_string;
        #ifndef NDEBUG
        const std::string m_text;
        #endif
    };


    /**
     * Creates a string parse node out of a null-terminated string.
     * @param string the string to create a string parse node from.
     * @return a string parse node.
     */
    template <class Symbol, std::enable_if_t<!std::is_function_v<Symbol>, bool> = true>
    string_parse_node<Symbol> make_parse_node(const Symbol* string) {
        return string_parse_node<Symbol>(string);
    }


    /**
     * Creates a string parse node out of a string.
     * @param string the string to create a string parse node from.
     * @return a string parse node.
     */
    template <class Symbol>
    string_parse_node<Symbol> make_parse_node(const std::basic_string<Symbol>& string) {
        return string_parse_node<Symbol>(string);
    }


    /**
     * Creates a string parse node out of a null-terminated string.
     * @param string the string to create a string parse node from.
     * @return a string parse node.
     */
    template <class Symbol>
    string_parse_node<Symbol> terminal(const Symbol* string) {
        return make_parse_node(string);
    }


    /**
     * Creates a string parse node out of a string.
     * @param string the string to create a string parse node from.
     * @return a string parse node.
     */
    template <class Symbol>
    string_parse_node<Symbol> terminal(const std::basic_string<Symbol>& string) {
        return make_parse_node(string);
    }


} //namespace parserlib


#endif //PARSERLIB_STRING_PARSE_NODE_HPP
