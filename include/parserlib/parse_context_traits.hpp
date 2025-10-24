#ifndef PARSERLIB_PARSE_CONTEXT_TRAITS_HPP
#define PARSERLIB_PARSE_CONTEXT_TRAITS_HPP


#include <string>
#include "void_text_position.hpp"


namespace parserlib {


    template <class String = std::string, bool DebugInfoEnabled = false> class parse_context_traits {
    public:
        using string_type = String;

        using value_type = typename String::value_type;

        using iterator_type = typename String::const_iterator;

        using text_position_type = void_text_position;

        using match_id_type = int;

        static constexpr bool debug_info_enabled = DebugInfoEnabled;

        template <class T> static auto to_lower(const T& value) {
            return value;
        }

        static void increment_parse_position(iterator_type& it, const iterator_type& end, text_position_type& tpos) {
            ++it;
            tpos.increment_column();
        }
    };


} //namespace parserlib


#endif // PARSERLIB_PARSE_CONTEXT_TRAITS_HPP
