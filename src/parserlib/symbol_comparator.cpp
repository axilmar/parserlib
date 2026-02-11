#include <cctype>
#include "parserlib/symbol_comparator.hpp"


namespace parserlib {


    symbol_value_type default_symbol_comparator::compare(symbol_value_type left, symbol_value_type right) {
        return left - right;
    }


    symbol_value_type case_insensitive_symbol_comparator::compare(symbol_value_type left, symbol_value_type right) {
        return std::tolower(left) - std::tolower(right);
    }


} //namespace parserlib
