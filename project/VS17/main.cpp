#include <stdexcept>
#include <type_traits>
#include <string>
#include <sstream>
#include <string_view>
#include <memory>
#include <map>
#include <vector>


namespace parserlib
{


    template <class ...T> 
    std::string to_string(T&&... args)
    {
        std::stringstream stream;
        (stream << ... << args);
        return stream.str();
    }


    enum class parse_result
    {
        accepted,
        rejected,
        left_recursion
    };


    enum class left_recursion_action
    {
        reject,
        accept
    };


    class exception : public std::exception
    {
    public:
        exception(std::string&& msg)
            : m_message(std::move(msg))
        {
        }

        virtual const char* what() const noexcept override
        {
            return m_message.c_str();
        }

    private:
        const std::string m_message;
    };


    class unsolvable_left_recursion : public exception
    {
    public:
        unsolvable_left_recursion()
            : exception("unsolvable left recursion")
        {
        }
    };


    class invalid_range : public exception
    {
    public:
        template <class T> 
        invalid_range(const T& min, const T& max)
            : exception(to_string("invalid range: ", min, " .. ", max))
        {
        }
    };


    template <class T> 
    class expression_type
    {
    public:
        typedef T type;
    };


    template <class T> 
    class expression_type<const T>
    {
    public:
        typedef typename expression_type<T>::type type;
    };


    template <class T> 
    class expression_type<T&>
    {
    public:
        typedef typename expression_type<T>::type type;
    };


    template <class T> 
    class expression_type<T&&>
    {
    public:
        typedef typename expression_type<T>::type type;
    };


    template <class T>
    using expression_type_t = typename expression_type<T>::type;


    class expression
    {
    public:
    };


    template <class T>
    inline constexpr bool is_expression = std::is_base_of_v<expression, T>;


    template <class T>
    inline constexpr bool has_expression_type = is_expression<expression_type_t<T>>;


	template <class ParseContext>
	class rule;


    template <class ParseContext> 
    class expression_interface : public expression
    {
    public:
        virtual ~expression_interface()
        {
        }

        virtual parse_result parse(ParseContext& pc) const = 0;
    };


    template <class ParseContext, class Expression> 
    class expression_wrapper : public expression_interface<ParseContext>
    {
    public:
        expression_wrapper(Expression&& expression)
            : m_expression(std::move(expression))
        {
        }

		virtual parse_result parse(ParseContext& pc) const override
        {
            return m_expression.parse(pc);
        }

    private:
        Expression m_expression;
    };


    template <class T> 
    class optional_expression;


    template <class T> 
    class loop_expression;


    template <class L, class R> 
    class sequence_expression;


    template <class T> 
    class unary_operations_base : public expression
    {
    public:
        T& self()
        {
            return *static_cast<T *>(this);
        }

        expression_type_t<T> self_expression()
        {
            return { self() };
        }

        optional_expression<expression_type_t<T>> operator -()
        {
            return { self_expression() };
        }

        loop_expression<expression_type_t<T>> operator *()
        {
            return { self_expression() };
        }

        sequence_expression<expression_type_t<T>, loop_expression<expression_type_t<T>>> operator +()
        {
            return { self_expression(), self_expression() };
        }
    };


    template <class T, class X> class unary_expression
        : public unary_operations_base<T>
    {
    public:
        unary_expression(X&& expression)
            : m_expression(std::move(expression))
        {
        }

	protected:
        const X& expression() const
        {
            return m_expression;
        }

    private:
        X m_expression;
    };


    template <class T> 
    class optional_expression
        : public unary_expression<optional_expression<T>, T>
    {
    public:
        using unary_expression<optional_expression<T>, T>::unary_expression;
		using unary_expression<optional_expression<T>, T>::expression;

		template <class ParseContext>
        parse_result parse(ParseContext& pc) const
        {
            pc.invoke(expression());
            return parse_result::accepted;
        }
    };


    template <class T> 
    class loop_expression
        : public unary_expression<loop_expression<T>, T>
    {
    public:
        using unary_expression<loop_expression<T>, T>::unary_expression;
        using unary_expression<loop_expression<T>, T>::expression;

        template <class ParseContext>
        parse_result parse(ParseContext& pc) const
        {
            for(bool loop = true; loop && pc.valid(); )
            {
                parse_result result = pc.invoke(expression());

                switch (result)
                {
					//continue the loop
                    case parse_result::accepted:
                        break;

					//end the loop
                    case parse_result::rejected:
                        loop = false;
                        break;

					//abort the loop
                    case parse_result::left_recursion:
                        return parse_result::left_recursion;
                }
            }

            return parse_result::accepted;
        }
    };


    template <class T> 
    class expression_reference 
        : public unary_operations_base<expression_reference<T>>
    {
    public:
        expression_reference(T& expression)
            : m_expression(expression)
        {
        }

		template <class ParseContext>
        parse_result parse(ParseContext& pc) const
        {
            return m_expression.parse(pc);
        }

	protected:
		const T& expression() const
		{
			return m_expression;
		}

    private:
        T& m_expression;
    };


    template <class T> 
    expression_reference<T> make_expression_reference(T& expression)
    {
        return { expression };
    }


    template <class T> 
    class terminal_expression 
        : public unary_operations_base<terminal_expression<T>>
    {
    public:
		terminal_expression(const T& value) : m_value(value)
		{
		}

		terminal_expression(T&& value) : m_value(std::move(value))
        {
        }

		template <class ParseContext>
        parse_result parse(ParseContext& pc) const
        {
            return pc.parse(m_value) ? parse_result::accepted : parse_result::rejected;
        }

    private:
        T m_value;
    };


    template <class T> 
    terminal_expression<T> terminal(const T& value)
    {
        return { value };
    }


    template <> 
    class expression_type<char>
    {
    public:
        typedef terminal_expression<char> type;
    };


    inline bool symbol_equals(char l, char r)
    {
        return l == r;
    }


    inline size_t symbol_length(char c)
    {
        return 1;
    }


    template <class T> 
    class range_symbol
    {
    public:
        range_symbol(const T& min, const T& max)
            : m_min(min), m_max(max)
        {
            test_good();
        }

        bool good() const
        {
            return m_min <= m_max;
        }

        void test_good() const
        {
            if (!good())
            {
                throw invalid_range(m_min, m_max);
            }
        }

        template <class V> 
        bool contains(const V& v) const
        {
            return v >= m_min && v <= m_max;
        }

    private:
        const T m_min, m_max;
    };


    template <class T>
    terminal_expression<range_symbol<T>> range(const T& min, const T& max)
    {
        return range_symbol<T>(min, max);
    }


    template <class T> 
    class expression_type<range_symbol<T>>
    {
    public:
        typedef terminal_expression<range_symbol<T>> type;
    };


    template <class T, class V> 
    bool symbol_equals(const range_symbol<T> &r, const V& v)
    {
        return r.contains(v);
    }


    template <class T> 
    size_t symbol_length(const range_symbol<T> &r)
    {
        return 1;
    }


    template <class T, class L, class R> 
    class binary_expression 
        : public unary_operations_base<T>
    {
    public:
        binary_expression(L&& left, R&& right)
            : m_left(std::move(left)), m_right(std::move(right))
        {
        }

	protected:
        const L& left_expression() const
        {
            return m_left;
        }

		L& left_expression()
		{
			return m_left;
		}

		const R& right_expression() const
        {
            return m_right;
        }

		R& right_expression()
		{
			return m_right;
		}

	private:
        L m_left;
        R m_right;
    };


    template <class L, class R> 
    class sequence_expression 
        : public binary_expression<sequence_expression<L, R>, L, R>
    {
    public:
        using binary_expression<sequence_expression<L, R>, L, R>::binary_expression;
        using binary_expression<sequence_expression<L, R>, L, R>::left_expression;
        using binary_expression<sequence_expression<L, R>, L, R>::right_expression;

		template <class ParseContext>
        parse_result parse(ParseContext& pc) const
        {
			//invoke the left expression
            parse_result result = pc.invoke(left_expression());

			//the left expression was successful; 
			//invoke the right expression
			if (result == parse_result::accepted)
			{
				result = pc.invoke(right_expression());
			}
            
			return result;
        }
    };


    template <class L, class R, typename = std::enable_if_t<has_expression_type<L> && has_expression_type<R>>>
    sequence_expression<expression_type_t<L>, expression_type_t<R>> operator >> (L&& left, R&& right)
    {
        return { expression_type_t<L>(std::forward<L>(left)), expression_type_t<R>(std::forward<R>(right)) };
    }


    template <class L, class R> class choice_expression 
        : public binary_expression<choice_expression<L, R>, L, R>
    {
    public:
        using binary_expression<choice_expression<L, R>, L, R>::binary_expression;
        using binary_expression<choice_expression<L, R>, L, R>::left_expression;
        using binary_expression<choice_expression<L, R>, L, R>::right_expression;

        template <class ParseContext> 
        parse_result parse(ParseContext& pc) const
        {
            //invoke the left expression
            parse_result result = pc.invoke(left_expression());

			//the left expression failed;
			//invoke the right expression
			if (result != parse_result::accepted)
			{
				result = pc.invoke(right_expression());
			}

            return result;
        }
    };


    template <class L, class R, typename = std::enable_if_t<has_expression_type<L> && has_expression_type<R>>>
    choice_expression<expression_type_t<L>, expression_type_t<R>> operator | (L&& left, R&& right)
    {
        return { expression_type_t<L>(std::forward<L>(left)), expression_type_t<R>(std::forward<R>(right)) };
    }


    template <class ParseContext = default_parse_context> 
    class rule;


    template <class InputIt> 
    class parse_context
    {
    public:
        typedef typename InputIt::value_type value_type;

        parse_context(const InputIt begin, const InputIt end)
            : m_it(begin), m_end(end)
        {
        }

        bool ended() const
        {
            return m_it == m_end;
        }

        bool valid() const
        {
            return !ended();
        }

        template <class T>
        bool parse(const T& symbol)
        {
            if (valid() && symbol_equals(symbol, *m_it))
            {
                m_it = std::next(m_it, symbol_length(symbol));
                return true;
            }
            return false;
        }

		template <class T>
		parse_result invoke(const T &expression)
		{
			//keep the start state locally so that it can be later restored
			const auto start_state = get_state();

			//invoke the expression
			const parse_result result = expression.parse(*this);
			
			//if the expression resulted in error, restore the state
			if (result != parse_result::accepted)
			{
				set_state(start_state);
			}
			
			return result;
		}

		std::basic_string_view<value_type> remaining_input() const
		{
			return ended() ?
				std::basic_string_view<value_type>() :
				std::basic_string_view<value_type>(&*m_it, (size_t)std::distance(m_it, m_end));
		}

	private:
		class state
		{
		public:
			const InputIt m_it;

			state(const InputIt& it)
				: m_it(it)
			{
			}
		};

		InputIt m_it;
        const InputIt m_end;

		state get_state() const
		{
			return state(m_it);
		}

		void set_state(const state& s)
		{
			m_it = s.m_it;
		}
	};


    template <class T> 
    parse_context<typename T::const_iterator> make_parse_context(T& container)
    {
        return { container.begin(), container.end() };
    }


    typedef parse_context<std::string::const_iterator> default_parse_context;


    template <class ParseContext>
    class rule_reference : public expression_reference<rule<ParseContext>>
    {
    public:
        using expression_reference<rule<ParseContext>>::expression_reference;

		parse_result parse(ParseContext& pc) const
		{
            //TODO
			return parse_result::rejected;
		}
    };


	template <class T> 
	class expression_unique_pointer : public std::unique_ptr<T>
	{
	public:
		using std::unique_ptr<T>::unique_ptr;

		template <class ParseContext>
		parse_result parse(ParseContext& pc) const
		{
			return std::unique_ptr<T>::get()->parse(pc);
		}
	};


    template <class ParseContext> class rule 
        : public unary_operations_base<rule<ParseContext>>
    {
    public:
        template <class T> 
        rule(T&& expression)
            : m_expression(std::make_unique<expression_wrapper<ParseContext, expression_type_t<T>>>(expression_type_t<T>(std::move(expression))))
        {
        }

        rule(rule& r)
            : m_expression(std::make_unique<expression_wrapper<ParseContext, rule_reference<ParseContext>>>(r))
        {
		}

        parse_result parse(ParseContext& pc) const
        {
            //TODO
            return parse_result::rejected;
        }

    private:
        expression_unique_pointer<expression_interface<ParseContext>> m_expression;

        //invoke expression
        parse_result invoke_expression(ParseContext& pc) const
        {
            parse_result result = pc.invoke(m_expression);
            if (result == parse_result::accepted)
            {
                pc.update_active_position(*this);
            }
            return result;
        }
    };


    template <class ParseContext> 
    class expression_type<rule<ParseContext>>
    {
    public:
        typedef rule_reference<ParseContext> type;
    };


} //namespace parserlib


#include <iostream>
using namespace parserlib;


extern rule<> expr;


auto num = -terminal('+') >> +range('0', '9');


rule<> val = /*'(' >> expr >> ')'
           | */num;


rule<> mul = mul >> '*' >> val
           | mul >> '/' >> val
           | val;


rule<> add = add >> '+' >> mul
           | add >> '-' >> mul
           | mul;


rule<> expr = add;


int main(int argc, char* argv[])
{
    std::string input = "1*2+3";

    auto pc = make_parse_context(input);

    auto pr = expr.parse(pc);

    std::cout << (int)pr << std::endl;
    std::cout << pc.remaining_input() << std::endl;

    system("pause");
    return 0;
}
