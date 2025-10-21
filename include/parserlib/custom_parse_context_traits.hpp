#ifndef PARSERLIB_CUSTOM_PARSE_CONTEXT_TRAITS_HPP
#define PARSERLIB_CUSTOM_PARSE_CONTEXT_TRAITS_HPP


#include <cctype>
#include "void_position.hpp"


namespace parserlib {


    class position {
    public:
        size_t line() const {
            return m_line;
        }

        size_t column() const {
            return m_column;
        }

        void increment_column() {
            ++m_column;
        }

        void increment_column(size_t count) {
            m_column += count;
        }

        void increment_line() {
            ++m_line;
            m_column = 1;
        }

    private:
        size_t m_line{1};
        size_t m_column{1};
    };


    class void_to_lower {
    public:
        template <class T> const T& operator ()(const T& v) const {
            return v;
        }
    };


    class to_lower {
    public:
        template <class T> int operator ()(const T& v) const {
            return std::tolower((int)v);
        }
    };


    template <class String, class MatchId = int, class Position = void_position, class ToLower = void_to_lower> class custom_parse_context_traits {
    public:
        using string_type = String;

        using value_type = typename String::value_type;

        using iterator_type = typename String::const_iterator;

        using position_type = Position;

        using match_id_type = MatchId;

        using to_lower_type = ToLower;

        template <class T> static auto to_lower(const T& value) {
            return to_lower_type()(value);
        }
    };


} //namespace parserlib


#endif // PARSERLIB_CUSTOM_PARSE_CONTEXT_TRAITS_HPP
