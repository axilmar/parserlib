#ifndef PARSERLIB__EVENT__HPP
#define PARSERLIB__EVENT__HPP


#include "expression_type.hpp"
#include "traits.hpp"


namespace parserlib
{


	/**
		Class that represents an event.
		@param T type of expression to invoke the event for.
		@param F type of function to invoke as an event handler.
	*/
	template <typename T, typename F>
	class event : public expression
	{
	public:
		/**
			Constructor.
			@param expression expression.
			@param event_handler event_handler.
		*/
		template <typename T, typename F>
		event(T&& expression, F&& event_handler)
			: m_expression(std::forward<T>(expression))
			, m_event_handler(std::forward<F>(event_handler))
		{
		}

		/**
			Parses the expression, and invokes the event handler.
			@param pc parse context.
			@return event handler result.
		*/
		template <typename ParseContext>
		parse_result parse(ParseContext& pc) const
		{
			const auto start_position = pc.start_position;
			parse_result result = m_expression.parse(pc);
			const auto prev_start_position = pc.start_position;
			pc.start_position = start_position;
			result = m_event_handler(pc, result);
			pc.start_position = prev_start_position;
			return result;
		}

	private:
		T m_expression;
		F m_event_handler;
	};


	/**
		Operator that creates a event handler out of an expression.
		@param expression expression to create a event for.
		@param event_handler function to inoke.
		@return the event expression.
	*/
	template <typename T, typename F, typename = std::enable_if_t<has_expression_type_v<T> && is_callable_v<F>>>
	event<expression_type_t<T>, F> operator >= (T&& expression, F&& event_handler)
	{
		return { expression_type_t<T>(std::forward<T>(expression)), std::forward<F>(event_handler) };
	}


} //namespace parserlib


#endif //PARSERLIB__EVENT__HPP
