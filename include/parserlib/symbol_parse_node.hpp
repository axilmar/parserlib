#ifndef PARSERLIB_SYMBOL_PARSE_NODE_HPP
#define PARSERLIB_SYMBOL_PARSE_NODE_HPP


#include <type_traits>
#include <cstdint>
#include "string_util.hpp"
#include "parse_node.hpp"


#ifndef char8_t
#define char8_t unsigned char
#endif


namespace parserlib {


    template <class T>
    class symbol_parse_node : public parse_node<symbol_parse_node<T>> {
    public:
        symbol_parse_node(const T& symbol)
            : parse_node<symbol_parse_node<T>>(get_type(symbol))
            , m_symbol(symbol)
        {
        }

        bool parse(parse_context_interface& pc) const {
            return pc.parse_symbol(static_cast<int>(m_symbol));
        }

    private:
        const T m_symbol;

        static std::string get_type(const T& symbol) {
            if constexpr (std::is_same_v<char, T> || std::is_same_v<unsigned char, T> || std::is_same_v<signed char, T> || std::is_same_v<char8_t, T>) {
                std::stringstream stream;
                stream << '\'' << symbol << '\'';
                return stream.str();
            }
            else if constexpr (std::is_same_v<char16_t, T> || std::is_same_v<char32_t, T>) {
                std::stringstream stream;
                stream << "\'\\u" << hex(symbol) << '\'';
                return stream.str();
            }
            else if constexpr (std::is_enum_v<T>) {
                return id_name(symbol);
            }
            else {
                std::stringstream stream;
                stream << symbol;
                return stream.str();
            }
        }
    };


    template <class T>
    auto terminal(const T& symbol) {
        return symbol_parse_node<T>(symbol);
    }


} //namespace parserlib


#endif //PARSERLIB_SYMBOL_PARSE_NODE_HPP
