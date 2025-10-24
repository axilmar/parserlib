#ifndef PARSERLIB_CUSTOM_PARSE_CONTEXT_TRAITS_HPP
#define PARSERLIB_CUSTOM_PARSE_CONTEXT_TRAITS_HPP


#include <cctype>
#include <algorithm>
#include <string>
#include <sstream>
#include "void_text_position.hpp"


namespace parserlib {


    class text_position {
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

        std::string to_string() const {
            std::stringstream stream;
            stream << '[' << m_line << ", " << m_column << ']';
            return stream.str();
        }

    private:
        size_t m_line{1};
        size_t m_column{1};
    };


    class void_to_lower_converter {
    public:
        template <class T> static const T& to_lower(const T& v) {
            return v;
        }
    };


    class to_lower_converter {
    public:
        template <class T> static int to_lower(const T& v) {
            return std::tolower((int)v);
        }
    };


    class void_new_line_parser {
    public:
        template <class Iterator, class TextPosition>
        static void parse_new_line(Iterator& it, const Iterator& end, TextPosition& tpos) {
            ++it;
            tpos.increment_column();
        }
    };


    class new_line_parser {
    public:
        template <class Iterator, class TextPosition>
        static void parse_new_line(Iterator& it, const Iterator& end, TextPosition& tpos) {
            const size_t newline_span = check_new_line(it, end);
            if (newline_span) {
                    it += newline_span;
                    tpos.increment_line();
            }
            else {
                ++it;
                tpos.increment_column();
            }
        }

    private:
        template <class Iterator, class TextPosition>
        size_t check_new_line(Iterator& it, const Iterator& end) {
            const Iterator next = std::next(it);
            if (*it == '\n') {
                if (next == end || *next != '\r') {
                    return 1;
                }
                return 2;

            }
            if (*it == '\r') {
                if (next == end || *next != '\n') {
                    return 1;
                }
                return 2;
            }
            return 0;
        }
    };


    template <class String, class MatchId = int, class TextPosition = void_text_position, class ToLowerConverter = void_to_lower_converter, class NewLineParser = void_new_line_parser, bool DebugInfoEnabled = false> class custom_parse_context_traits {
    public:
        using string_type = String;

        using value_type = typename String::value_type;

        using iterator_type = typename String::const_iterator;

        using text_position_type = TextPosition;

        using match_id_type = MatchId;

        static constexpr bool debug_info_enabled = DebugInfoEnabled;

        template <class T> static auto to_lower(const T& value) {
            return ToLowerConverter::to_lower(value);
        }

        static void increment_parse_position(iterator_type& it, const iterator_type& end, text_position_type& tpos) {
            NewLineParser::parse_new_line(it, end, tpos);
        }
    };


} //namespace parserlib


#endif // PARSERLIB_CUSTOM_PARSE_CONTEXT_TRAITS_HPP
