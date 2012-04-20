#ifndef PARSERLIB_HPP
#define PARSERLIB_HPP


#include "rule.hpp"


namespace parserlib {


/** parses an input.
    @param input input to parse.
    @param grammar root rule of the grammar.
    @param ws whitespace rule.
    @return true on success, false on failure.
 */
bool parse(input_buffer &input, rule &grammar, rule &ws); 


} //namespace parserlib


#endif //PARSERLIB_HPP
