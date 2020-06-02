#ifndef PARSERLIB__TERMINAL__HPP
#define PARSERLIB__TERMINAL__HPP


#include "parse_result.hpp"
#include "expression_with_unary_operators.hpp"
#include "traits.hpp"


namespace parserlib
{


    /**
        The default implementation of parse terminal for non pointer types that are not containers.
        @param value value to parse.
        @param position current position; on success, the position must have been incremented.
        @param end end position.
        @return true on success, false on failure.
     */
    template <typename T, typename It, typename = std::enable_if_t<!is_container_v<T>>>
    bool parse_terminal(const T value, It& position, const It end)
    {
        if (position != end && *position == value)
        {
            ++position;
            return true;
        }
        return false;
    }


    /**
        The default implementation of parse terminal for pointer types.
        It assumes the data are null-terminated.
        @param data data to parse.
        @param position current position; on success, the position must have been incremented.
        @param end end position.
        @return true on success, false on failure.
     */
    template <typename T, typename It>
    bool parse_terminal(const T* data, It& position, const It end)
    {
        const T* dataIt = data;
        It positionIt = position;

        for (;;)
        {
            //if the last element of the data is reached,
            //then parsing was successful.
            if (!*dataIt)
            {
                position = positionIt;
                return true;
            }

            //if the end of the input is reached,
            //there was an error
            if (positionIt == end)
            {
                break;
            }

            //if elements differ, there is an error
            if (*dataIt != *positionIt)
            {
                break;
            }

            //next elements
            ++dataIt;
            ++positionIt;
        }

        return false;
    }


    /**
        The default implementation of parse terminal for container types.
        @param container to parse.
        @param position current position; on success, the position must have been incremented.
        @param end end position.
        @return true on success, false on failure.
     */
    template <typename T, typename It, typename = std::enable_if_t<is_container_v<T>>>
    bool parse_terminal(const T& container, It& position, const It end)
    {
        auto containerIt = container.begin();
        It positionIt = position;

        for (;;)
        {
            //if the last element of the container is reached,
            //then parsing was successful.
            if (containerIt == container.end())
            {
                position = positionIt;
                return true;
            }

            //if the end of the input is reached,
            //there was an error
            if (positionIt == end)
            {
                break;
            }

            //if elements differ, there is an error
            if (*containerIt != *positionIt)
            {
                break;
            }

            //next elements
            ++containerIt;
            ++positionIt;
        }

        return false;
    }


    /**
        A parser that parses a terminal.
        @param T type of terminal to parse.
     */
    template <typename T>
    class terminal : public expression_with_unary_operators<terminal<T>>
    {
    public:
        /**
            Constructor from value.
            @param value to parse.
         */
        terminal(const T& value)
            : m_value(value)
        {
        }

        /**
            Parses the given item.
            It checks if the value matches the input at current position.
            If so, then the parsing position is advanced.
            @param pc parse context.
            @return accepted if the value matches the input, rejected otherwise.
         */
        template <typename ParseContext>
        parse_result parse(ParseContext& pc) const
        {
            if (parse_terminal(m_value, pc.position, pc.end))
            {
                return parse_result::accepted;
            }
            return parse_result::rejected;
        }

    private:
        //Element to parse.
        T m_value;
    };


    /**
        Specialization of expression type for character terminal.
     */
    template <> class expression_type<char>
    {
    public:
        ///the parser for character is atom.
        typedef terminal<char> type;
    };


} //namespace parserlib


#endif //PARSERLIB__TERMINAL__HPP
