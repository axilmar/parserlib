#ifndef PARSERLIB__RANGE__HPP
#define PARSERLIB__RANGE__HPP


#include <stdexcept>
#include "expression_type.hpp"


namespace parserlib
{


    /**
        A parser that parses a range of elements.
        @param T type of element to parse.
     */
    template <typename T = char>
    class range : public expression
    {
    public:
        /**
            Constructor from range.
            @param min_element min element.
            @param max_element max element.
            @exception std::logic_error thrown if min > max.
         */
        range(const T& min_element, const T& max_element)
            : m_min_element(min_element), m_max_element(max_element)
        {
            if (m_min_element > m_max_element)
            {
                throw std::logic_error("min greater than max");
            }
        }

        /**
            Parses the given item.
            It checks if the input at current position is within range.
            If so, then the parsing position is advanced.
            @param pc parse context.
            @return accepted if the input is within range, rejected otherwise.
         */
        template <typename ParseContext>
        parse_result parse(ParseContext& pc) const
        {
            if (pc.valid() && *pc.position >= m_min_element && *pc.position <= m_max_element)
            {
                ++pc.position;
                return parse_result::accepted;
            }
            return parse_result::rejected;
        }

    private:
        //range.
        T m_min_element, m_max_element;
    };


} //namespace parserlib


#endif //PARSERLIB__RANGE__HPP
