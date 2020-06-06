#ifndef PARSERLIB__TERMINAL__HPP
#define PARSERLIB__TERMINAL__HPP


#include <set>
#include "parse_result.hpp"
#include "expression_type.hpp"
#include "traits.hpp"


namespace parserlib
{


	/**
		A range type.
	 */
	template <typename T> class range
	{
	public:
		///min value.
		const T min;

		///max value.
		const T max;

		/**
			Constructor.
			@param min_ min value.
			@param max_ max value.
		 */
		range(const T& min_, const T& max_)
			: min(min_), max(max_)
		{
		}
	};


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
		The terminal parser for a range.
		@param r range.
		@param position current position; on success, the position must have been incremented.
		@param end end position.
		@return true on success, false on failure.
		*/
	template <typename T, typename It>
	bool parse_terminal(const range<T>& r, It& position, const It end)
	{
		if (position != end && *position >= r.min && *position <= r.max)
		{
			++position;
			return true;
		}
		return false;
	}


	/**
		The terminal parser for a set.
		@param s set.
		@param position current position; on success, the position must have been incremented.
		@param end end position.
		@return true on success, false on failure.
	*/
	template <typename T, typename Pr, typename Alloc, typename It>
	bool parse_terminal(const std::set<T, Pr, Alloc>& s, It& position, const It end)
	{
		if (position != end && s.find(*position) != s.end())
		{
			++position;
			return true;
		}
		return false;
	}


	/**
        A parser that parses a terminal.
        @param T type of terminal to parse.
     */
    template <typename T>
    class terminal : public expression
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
            Constructor from value.
            @param value to parse.
         */
        terminal(T&& value)
            : m_value(std::move(value))
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
        ///terminal parser for character.
        typedef terminal<char> type;
    };


	/**
		Specialization of expression type for char16_t terminal.
	 */
	template <> class expression_type<char16_t>
	{
	public:
		///terminal parser for character.
		typedef terminal<char16_t> type;
	};


	/**
		Specialization of expression type for char32_t terminal.
	*/
	template <> class expression_type<char32_t>
	{
	public:
		///terminal parser for character.
		typedef terminal<char32_t> type;
	};


	/**
		Specialization of expression type for null-terminated string terminal.
	*/
	template <typename T> class expression_type<const T *>
	{
	public:
		///terminal parser for null-terminated string.
		typedef terminal<const T *> type;
	};


	/**
		Specialization of expression type for null-terminated string terminal.
	 */
	template <typename T> class expression_type<T *>
	{
	public:
		///terminal parser for null-terminated string.
		typedef terminal<T *> type;
	};


	/**
		Specialization of expression type for basic string terminal.
	*/
	template <typename T, typename Traits> class expression_type<std::basic_string<T, Traits>>
	{
	public:
		///terminal parser for string.
		typedef terminal<std::basic_string<T, Traits>> type;
	};


	/**
		Specialization of expression type for basic string view terminal.
	*/
	template <typename T, typename Traits> class expression_type<std::basic_string_view<T, Traits>>
	{
	public:
		///terminal parser for string view.
		typedef terminal<std::basic_string_view<T, Traits>> type;
	};


	/**
		Specialization of expression type for range terminal.
	 */
	template <typename T> class expression_type<range<T>>
	{
	public:
		///terminal parser for range.
		typedef terminal<range<T>> type;
	};


	/**
		Specialization of expression type for set terminal.
	*/
	template <typename T, typename Pr, typename Alloc> class expression_type<std::set<T, Pr, Alloc>>
	{
	public:
		///terminal parser for set.
		typedef terminal<std::set<T, Pr, Alloc>> type;
	};


	/**
		Creates a set terminal parser out of the characters
		of the given null-terminated string.
		@param elements null-terminated string with set characters.
		@return set terminal parser.
	 */
	template <typename T>
	terminal<std::set<T>> one_of(const T* elements)
	{
		const T* elements_end = elements;
		for (; *elements_end; ++elements_end) {}
		return { std::set<T>(elements, elements_end) };
	}


} //namespace parserlib


#endif //PARSERLIB__TERMINAL__HPP
