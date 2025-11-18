#ifndef PARSERLIB_TERMINAL_FUNCTIONS_HPP
#define PARSERLIB_TERMINAL_FUNCTIONS_HPP


#include <string>
#include <sstream>
#include "is_char.hpp"
#include "id_name.hpp"


namespace parserlib {


    template <class Symbol>
    std::string symbol_text(const Symbol& symbol) {
        std::stringstream stream;
        if constexpr (is_char_v<Symbol>) {
            stream << "'" << symbol << "'";
        }
        else {
            stream << id_name<Symbol>::get(symbol);
        }
        return stream.str();
    }


} //namespace parserlib


#endif //PARSERLIB_TERMINAL_FUNCTIONS_HPP
