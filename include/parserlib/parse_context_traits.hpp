#ifndef PARSERLIB_PARSE_CONTEXT_TRAITS_HPP
#define PARSERLIB_PARSE_CONTEXT_TRAITS_HPP


namespace parserlib {


    template <class String> class parse_context_traits {
    public:
        using string_type = String;

        using value_type = typename String::value_type;

        using iterator_type = typename String::const_iterator;

        class position {
        public:
            size_t line() const {
                return 0;
            }

            size_t column() const {
                return 0;
            }

            void increment_column() {
            }

            void increment_column(size_t count) {
            }

            void increment_line() {
            }
        };

        template <class T> static auto to_lower(const T& value) {
            return value;
        }
    };


} //namespace parserlib


#endif // PARSERLIB_PARSE_CONTEXT_TRAITS_HPP
