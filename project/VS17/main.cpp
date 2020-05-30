#include <stdexcept>
#include <type_traits>
#include <string>
#include <sstream>
#include <string_view>
#include <memory>


namespace parserlib
{


    template <class ...T> std::string to_string(T&&... args)
    {
        std::stringstream stream;
        (stream << args)...;
        return stream.str();
    }


    enum class parse_result
    {
        accepted,
        rejected,
        rejected_left_recursion
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
        template <class T> invalid_range(const T& min, const T& max)
            : exception(to_string("invalid range: ", min, " .. ", max))
        {
        }
    };


    template <class T> class expression_type
    {
    public:
        typedef T type;
    };


    template <class T> class expression_type<const T>
    {
    public:
        typedef typename expression_type<T>::type type;
    };


    template <class T> class expression_type<T&>
    {
    public:
        typedef typename expression_type<T>::type type;
    };


    template <class T> class expression_type<T&&>
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


    template <class ParseContext> class expression_interface : public expression
    {
    public:
        virtual ~expression_interface()
        {
        }

        virtual parse_result parse(ParseContext& pc, left_recursion_action lra) = 0;
    };


    template <class ParseContext, class Expression> 
    class expression_wrapper : public expression_interface<ParseContext>
    {
    public:
        expression_wrapper(Expression&& expression)
            : m_expression(std::move(expression))
        {
        }

        virtual parse_result parse(ParseContext& pc, left_recursion_action lra) override
        {
            return m_expression.parse(pc, lra);
        }

    private:
        Expression m_expression;
    };


    template <class T> class expression_reference : public expression
    {
    public:
        expression_reference(T& expression)
            : m_expression(expression)
        {
        }

        template <class ParseContext> 
        parse_result parse(ParseContext& pc, left_recursion_action lra)
        {
            return m_expression.parse(pc, lra);
        }

    private:
        T& m_expression;
    };


    template <class T> expression_reference<T> make_expression_reference(T& expression)
    {
        return { expression };
    }


    template <class T> class terminal_expression : public expression
    {
    public:
        terminal_expression(T&& value) : m_value(std::move(value))
        {
        }

        template <class ParseContext>
        parse_result parse(ParseContext& pc, left_recursion_action lra)
        {
            return pc.parse(m_value) ? parse_result::accepted : parse_result::rejected;
        }

    private:
        T m_value;
    };


    template <class T> terminal_expression<T> terminal(const T& value)
    {
        return { value };
    }


    template <> class expression_type<char>
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


    template <class T> class range_symbol
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
            if (!good()) throw invalid_range(m_min, m_max);
        }

        template <class V> bool contains(const V& v) const
        {
            return v >= m_min && v <= m_max;
        }

    private:
        const T m_min, m_max;
    };


    template <class T>
    terminal<range_symbol<T>> range(const T& min, const T& max)
    {
        return { min, max };
    }


    template <class T> class expression_type<range_symbol<T>>
    {
    public:
        typedef terminal<range_symbol<T>> type;
    };


    template <class T, class V> bool symbol_equals(const range_symbol<T> &r, const V& v)
    {
        return r.contains(v);
    }


    template <class T> size_t symbol_length(const range_symbol<T> &r)
    {
        return 1;
    }


    template <class L, class R> class binary_expression : public expression
    {
    public:
        binary_expression(L&& left, R&& right)
            : m_left(left), m_right(right)
        {
        }

    protected:
        L m_left;
        R m_right;
    };


    template <class L, class R> class sequence_expression : public binary_expression<L, R>
    {
    public:
        using binary_expression<L, R>::binary_expression;

        template <class ParseContext> 
        parse_result parse(ParseContext& pc, left_recursion_action lra)
        {
            //parse the left expression
            parse_result left_res = m_left.parse(pc, lra);

            //if the left expression rejected the parse due to left recursion,
            //abort the sequence_expression and give a chance to the caller
            //to solve the left recursion
            if (left_res == parse_result::rejected_left_recursion)
            {
                return parse_result::rejected_left_recursion;
            }

            //parse the right expression
            return m_right.parse(pc, lra);
        }
    };


    template <class L, class R, typename = std::enable_if_t<has_expression_type<L> && has_expression_type<R>>>
    sequence_expression<expression_type_t<L>, expression_type_t<R>>, operator >> (L&& left, R&& right)
    {
        return { expression_type_t<L>(std::forward<L>(left)), expression_type_t<R>(std::forward<R>(right)) };
    }


    template <class L, class R> class choice_expression : public binary_expression<L, R>
    {
    public:
        using binary_expression<L, R>::binary_expression;

        template <class ParseContext> 
        parse_result parse(ParseContext& pc, left_recursion_action lra)
        {
            //keep the current state in order to rewind the parser
            //if the left expression fails
            auto start_state = pc.get_state();

            //parse the left expression
            parse_result left_res = m_left.parse(pc, lra);

            //if the left expression was accepted, do nothing else
            if (left_res == parse_result::accepted)
            {
                return parse_result::accepted;
            }

            //rewind the parse state in order to try the right expression
            pc.set_state(start_state);

            //parse the right expression
            parse_result right_res = m_right.parse(pc, lra);

            //if the left expression result was 'rejected',
            //then there is no need to handle left recursion,
            //and therefore return the right result as is
            if (left_res == parse_result::rejected)
            {
                return right_res;
            }

            /******************************************************************
             handle left recursion as a result of the left expression
             ******************************************************************/

            //if the right expression failed due to left recursion,
            //it means this part of the grammar is invalid, so abort
            if (right_res == parse_result::rejected_left_recursion)
            {
                throw unsolvable_left_recursion();
            }

            //if the right expression failed,
            //return the left recursion to the caller,
            //which might try to solve it
            if (right_res == parse_result::rejected)
            {
                return parse_result::rejected_left_recursion;
            }

            //the right expression returned 'accepted',
            //and therefore the left expression can now be resolved;
            //parse it in a loop in order to capture repetition
            for (bool loop = true; loop; )
            {
                auto start_state = pc.get_state();

                //parse the left expression again, this time accepting
                //any left recursion without processing the subexpressions
                parse_result left_res = m_left.parse(pc, left_recursion_action::accept);

                switch (left_res)
                {
                    //if parsing suceeded, continue parsing
                    case parse_result::accepted;
                        break;

                    //if parsing failed, stop the loop
                    case parse_result::rejected:
                        pc.set_state(start_state);
                        loop = false;
                        break;

                    //if there is still a left recursion, abort
                    case parse_result::rejected_left_recursion:
                        throw unsolvable_left_recursion();
                }
            }

            //return 'accepted' since the right expression returned 'accepted'
            return parse_result::accepted;
        }
    };


    template <class L, class R, typename = std::enable_if_t<has_expression_type<L> && has_expression_type<R>>>
    choice_expression<expression_type_t<L>, expression_type_t<R>>, operator | (L&& left, R&& right)
    {
        return { expression_type_t<L>(std::forward<L>(left)), expression_type_t<R>(std::forward<R>(right)) };
    }


    template <class InputIt> class parse_context
    {
    public:
        typedef typename InputIt::value_type value_type;

        class state
        {
        public:
            const InputIt& it() const
            {
                return m_it;
            }

        private:
            const InputIt m_it;

            state(const InputIt& it)
                : m_it(it)
            {
            }

            friend class parse_context<InputIt>;
        };

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

        state get_state() const
        {
            return state(m_it);
        }

        void set_state(const state& s)
        {
            m_it = s.m_it;
        }

    private:
        InputIt m_it;
        const InputIt m_end;
    };


    typedef parse_context<std::string_view::const_iterator> default_parse_context;


    template <class ParseContext = default_parse_context> class rule;


    template <class ParseContext>
    using rule_reference = expression_reference<rule<ParseContext>>;


    template <class ParseContext = default_parse_context> class rule : public expression
    {
    public:
        template <class T> rule(T&& expression)
            : m_expression(std::make_unique<expression_wrapper<ParseContext, T>>(std::move(expression)))
        {
        }

        rule(rule& r)
            : m_expression(std::make_unique<expression_wrapper<ParseContext, rule_reference<ParseContext>>>(r))
        {
        }

    private:
        std::unique_ptr<expression_interface<ParseContext>> m_expression;
    };


    template <class ParseContext> class expression_type<rule<ParseContext>>
    {
    public:
        typedef rule_reference<ParseContext> type;
    };


} //namespace parserlib


#include <iostream>


int main(int argc, char* argv[])
{
    return 0;
}
