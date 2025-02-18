#ifndef PARSERLIB_PARSER_HPP
#define PARSERLIB_PARSER_HPP


#include <type_traits>


namespace parserlib {


    template <class Derived> class parser;
    template <class Parser> class zero_or_more_parser;
    template <class Parser> class one_or_more_parser;
    template <class Parser> class optional_parser;
    template <class Parser> class logical_and_parser;
    template <class Parser> class logical_not_parser;
    template <class Derived> class parser;
    template <class ParseContext> class rule;
    template <class ParseContext> class rule_reference;


    template <class Derived> 
    const Derived& get_parser_wrapper(const parser<Derived>& parser) noexcept {
        return static_cast<const Derived&>(parser);
    }


    template <class ParseContext> 
    rule_reference<ParseContext> get_parser_wrapper(const parser<rule<ParseContext>>& parser) noexcept {
        return static_cast<const rule<ParseContext>&>(parser);
    }


    template <class Derived>
    using parser_wrapper_type = std::remove_const_t<std::remove_reference_t<decltype(get_parser_wrapper(*static_cast<const parser<Derived>*>(nullptr)))>>;


    template <class Derived>
    class parser {
    public:
        using parser_type = Derived;

        using this_parser_wrapper_type = parser_wrapper_type<Derived>;

        zero_or_more_parser<this_parser_wrapper_type> operator *() const {
            return get_parser_wrapper(*this);
        }

        one_or_more_parser<this_parser_wrapper_type> operator +() const {
            return get_parser_wrapper(*this);
        }

        optional_parser<this_parser_wrapper_type> operator -() const {
            return get_parser_wrapper(*this);
        }

        logical_and_parser<this_parser_wrapper_type> operator &() const {
            return get_parser_wrapper(*this);
        }

        logical_not_parser<this_parser_wrapper_type> operator !() const {
            return get_parser_wrapper(*this);
        }
    };


} //namespace parserlib


#endif //PARSERLIB_PARSER_HPP
