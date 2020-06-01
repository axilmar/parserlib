#ifndef PARSERLIB__ATOM__HPP
#define PARSERLIB__ATOM__HPP


#include "expression_with_unary_operators.hpp"


namespace parserlib
{


    /**
        A parser that parses a single element.
        @param T type of element to parse.
     */
    template <typename T = char>
    class atom : public expression_with_unary_operators<atom<T>>
    {
    public:
        /**
            Constructor from element.
            @param element to parse.
         */
        atom(const T& element)
            : m_element(element)
        {
        }

    private:
        //Element to parse.
        T m_element;
    };


    /**
        Makes an atom parser.
        @param element element to make an atom parser of.
        @return an atom parser instance.
     */
    template <typename T>
    atom<T> make_atom(const T& element)
    {
        return { element };
    }


    /**
        Makes an atom out of a character.
        @param c character to make an atom of.
        @return a character atom.
     */
    inline atom<char> make_atom(char c)
    {
        return { c };
    }


    /**
        Specialization of expression type for character.
     */
    template <> class expression_type<char>
    {
    public:
        ///the parser for character is atom.
        typedef atom<char> type;
    };


} //namespace parserlib


#endif //PARSERLIB__ATOM__HPP
