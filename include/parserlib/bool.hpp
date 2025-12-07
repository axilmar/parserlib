#ifndef PARSERLIB_BOOL_HPP
#define PARSERLIB_BOOL_HPP


#include "parse_node.hpp"


namespace parserlib {


    class bool_ : public parse_node<bool_> {
    public:
        bool_(bool value) : m_value(value) {
        }

        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            return m_value;
        }

    private:
        const bool m_value;
    };


} //namespace parserlib


#endif //PARSERLIB_BOOL_HPP
