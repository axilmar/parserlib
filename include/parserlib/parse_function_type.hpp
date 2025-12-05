#ifndef PARSERLIB_PARSE_FUNCTION_TYPE_HPP
#define PARSERLIB_PARSE_FUNCTION_TYPE_HPP


#include <functional>


namespace parserlib {


    class parse_context_interface;


    using parse_function_type = std::function<bool(parse_context_interface& pc)>;


} //namespace parserlib


#endif //PARSERLIB_PARSE_FUNCTION_TYPE_HPP
