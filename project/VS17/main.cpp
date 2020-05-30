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
    class expression_interface : public expression
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
        X& expression()
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

        template <class ParseContext>
        parse_result parse(ParseContext& pc, left_recursion_action lra)
        {
            auto start_state = pc.get_state();
            
            parse_result result = expression().parse(pc, lra);
            
            switch (result)
            {
                case parse_result::accepted:
                    break;

                case parse_result::rejected:
                    pc.set_state(start_state);
                    result = parse_result::accepted;
                    break;

                case parse_result::left_recursion:
                    pc.set_state(start_state);
                    break;
            }

            return result;
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
        parse_result parse(ParseContext& pc, left_recursion_action lra)
        {
            for(bool loop = true; loop && pc.valid(); )
            {
                auto start_state = pc.get_state();

                parse_result result = expression().parse(pc, lra);

                switch (result)
                {
                    case parse_result::accepted:
                        break;

                    case parse_result::rejected:
                        pc.set_state(start_state);
                        loop = false;
                        break;

                    case parse_result::left_recursion:
                        pc.set_state(start_state);
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
        parse_result parse(ParseContext& pc, left_recursion_action lra)
        {
            return m_expression.parse(pc, lra);
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
        L& left_expression()
        {
            return m_left;
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
        parse_result parse(ParseContext& pc, left_recursion_action lra)
        {
            auto start_state = pc.get_state();

            //parse the left expression
            parse_result result = left_expression().parse(pc, lra);

            switch (result)
            {
                //if the left expression succeeded, try the right one
                case parse_result::accepted:
                    result = right_expression().parse(pc, lra);

                    switch (result)
                    {
                        //sequence totally accepted
                        case parse_result::accepted:
                            break;

                        //sequence rejected; restore the state
                        case parse_result::rejected:
                        case parse_result::left_recursion:
                            pc.set_state(start_state);
                            break;

                    }
                    break;

                //if the left expression failed, restore the state
                case parse_result::rejected:
                case parse_result::left_recursion:
                    pc.set_state(start_state);
                    break;
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
        parse_result parse(ParseContext& pc, left_recursion_action lra)
        {
            auto start_state = pc.get_state();

            //parse the left expression
            parse_result result = left_expression().parse(pc, lra);

            switch (result)
            {
                //success from left expression
                case parse_result::accepted:
                    break;

                //failure from left expression; 
                //try the right expression
                case parse_result::rejected:
                    pc.set_state(start_state);
                    result = right_expression().parse(pc, lra);
                    break;

                //left recursion from left expression; 
                //try the right expression;
                //try to resolve the left recursion
                case parse_result::left_recursion:
                    pc.set_state(start_state);
                    result = right_expression().parse(pc, lra);

                    switch (result)
                    {
                        //success from right expression;
                        //try to resolve the left recursion
                        case parse_result::accepted:
                            for (bool loop = true; loop && pc.valid(); )
                            {
                                auto start_state = pc.get_state();
                                result = left_expression().parse(pc, left_recursion_action::accept);

                                switch (result)
                                {
                                    //left recursion resolved;
                                    //continue parsing
                                    case parse_result::accepted:
                                        break;

                                    //left recursion parsing ended
                                    case parse_result::rejected:
                                    case parse_result::left_recursion:
                                        pc.set_state(start_state);
                                        loop = false;
                                        break;
                                }
                            }
                            break;

                        //failure from right expression;
                        //pass the result to the caller
                        case parse_result::rejected:
                        case parse_result::left_recursion:
                            pc.set_state(start_state);
                            break;
                    }
                    break;
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

        bool add(rule<parse_context>& rule)
        {
            //find the rule in the recursion map
            const auto it = m_recursionMap.find(&rule);

            //if not found, then place it and return success
            if (it == m_recursionMap.end())
            {
                m_recursionMap[&rule].push_back(m_it);
                return true;
            }

            //if the last position for this rule is the same,
            //then we have a left recursion and the rule cannot be added
            if (it->second.back() == m_it)
            {
                return false;
            }

            //add the position
            it->second.push_back(m_it);

            //the position is successfully added
            return true;
        }

        void remove(rule<parse_context>& rule)
        {
            //find the rule
            const auto it = m_recursionMap.find(&rule);

            //if the rule is not found, do nothing
            if (it == m_recursionMap.end())
            {
                return;
            }

            //pop the last position
            it->second.pop_back();

            //if there are no more positions for this rule, remove the rule
            if (it->second.empty())
            {
                m_recursionMap.erase(it);
            }
        }

        std::basic_string_view<value_type> remaining_input() const
        {
            return ended() ? 
                   std::basic_string_view<value_type>() : 
                   std::basic_string_view<value_type>(&*m_it, (size_t)std::distance(m_it, m_end));
        }

    private:
        InputIt m_it;
        const InputIt m_end;
        std::map<rule<parse_context>*, std::vector<InputIt>> m_recursionMap;
    };


    template <class T> 
    parse_context<typename T::const_iterator> make_parse_context(T& container)
    {
        return { container.begin(), container.end() };
    }


    typedef parse_context<std::string::const_iterator> default_parse_context;


    template <class ParseContext>
    using rule_reference = expression_reference<rule<ParseContext>>;


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

        parse_result parse(ParseContext& pc, left_recursion_action lra = left_recursion_action::reject)
        {
            parse_result result;

            //try to add the rule for the current position
            const bool added = pc.add(*this);

            //if the rule was added, proceed with parsing of the subexpression
            if (added)
            {
                result = m_expression->parse(pc, left_recursion_action::reject);
                pc.remove(*this);
            }

            //else handle left recursion
            else
            {
                switch (lra)
                {
                    //reject the parse due to left recursion
                    case left_recursion_action::reject:
                        result = parse_result::left_recursion;
                        break;

                    //accept the parse due to left recursion
                    case left_recursion_action::accept:
                        result = parse_result::accepted;
                        break;
                }
            }

            return result;
        }

    private:
        std::unique_ptr<expression_interface<ParseContext>> m_expression;
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


auto num = +range('0', '9');


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
    std::string input = "1+2";

    auto pc = make_parse_context(input);

    auto pr = expr.parse(pc);

    std::cout << (int)pr << std::endl;
    std::cout << pc.remaining_input() << std::endl;

    system("pause");
    return 0;
}
