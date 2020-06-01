#ifndef PARSERLIB__RANGE__HPP
#define PARSERLIB__RANGE__HPP


#include <stdexcept>
#include "expression_with_unary_operators.hpp"


namespace parserlib
{


    /**
        A parser that parses a range of elements.
        @param T type of element to parse.
     */
    template <typename T = char>
    class range : public expression_with_unary_operators<range<T>>
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

    private:
        //range.
        T m_min_element, m_max_element;
    };


    /**
        Makes an range parser.
        @param min_element min element.
        @param max_element max element.
        @return an range parser instance.
     */
    template <typename T>
    range<T> make_range(const T& min_element, const T& max_element)
    {
        return { min_element, max_element };
    }


} //namespace parserlib


#endif //PARSERLIB__RANGE__HPP