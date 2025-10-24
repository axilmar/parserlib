#ifndef PARSERLIB_VOID_TEXT_POSITION_HPP
#define PARSERLIB_VOID_TEXT_POSITION_HPP


#include <string>


namespace parserlib {


    class void_text_position {
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

        std::string to_string() const {
            return {};
        }
    };


} //namespace parserlib


#endif // PARSERLIB_VOID_TEXT_POSITION_HPP
