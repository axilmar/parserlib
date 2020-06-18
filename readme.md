# Parserlib #

C++17 PEG parser library that supports left recursion.

## Calculator Example

### grammar
	
	#include "calculator_grammar.hpp"
	
	
	using namespace parserlib;
	
	
	namespace calculator
	{ 
	
	
	    static auto sign = one_of("-+");
	    static auto digits = +range('0', '9');
	    static auto num_basic_part = digits >> -('.' >> -digits) | '.' >> digits;
	    static auto num_exp_part = one_of("eE") >> -sign >> digits;
	
	
	    //number
	    static auto num = -sign >> num_basic_part >> -num_exp_part  == "num";
	
	
	    //whitespace
	    static auto ws = *terminal(' ');
	
	
	    //value
	    static auto val = ws >> ('(' >> expr >> ')' | num) >> ws;
	
	
	    //multiplication/division
	    static rule<> mul = mul >> '*' >> val                         == "mul"
	                      | mul >> '/' >> val                         == "div"
	                      | val;
	
	
	    //addition/subtraction
	    static rule<> add = add >> '+' >> mul                         == "add"
	                      | add >> '-' >> mul                         == "sub"
	                      | mul;
	
	
	    //expression
	    rule<> expr = add;
	
	
	} //namespace calculator

### abstract syntax tree

    //base class for expressions
    class ast_expr
    {
    public:
        virtual double eval() const = 0;
    };


    //number
    class ast_num : public ast_expr
    {
    public:
        ast_num(const double v)
            : m_value(v)
        {
        }

        virtual double eval() const
        {
            return m_value;
        }

    private:
        const double m_value;
    };


    //base class for binary expressions
    class ast_binary_expr : public ast_expr
    {
    public:
        ast_binary_expr(
            const std::shared_ptr<ast_expr>& left,
            const std::shared_ptr<ast_expr>& right)
            : m_left(left), m_right(right)
        {
        }

    protected:
        std::shared_ptr<ast_expr> m_left, m_right;
    };


    //division
    class ast_div : public ast_binary_expr
    {
    public:
        using ast_binary_expr::ast_binary_expr;

        virtual double eval() const
        {
            return m_left->eval() / m_right->eval();
        }
    };


    //multiplication
    class ast_mul : public ast_binary_expr
    {
    public:
        using ast_binary_expr::ast_binary_expr;

        virtual double eval() const
        {
            return m_left->eval() * m_right->eval();
        }
    };


    //subtraction
    class ast_sub : public ast_binary_expr
    {
    public:
        using ast_binary_expr::ast_binary_expr;

        virtual double eval() const
        {
            return m_left->eval() - m_right->eval();
        }
    };


    //addition
    class ast_add : public ast_binary_expr
    {
    public:
        using ast_binary_expr::ast_binary_expr;

        virtual double eval() const
        {
            return m_left->eval() + m_right->eval();
        }
    };

### binding of AST to grammar

    template <typename T>
    void create_binary_ast(std::vector<std::shared_ptr<ast_expr>>& ast_node_stack)
    {
        if (ast_node_stack.size() < 2)
        {
            throw std::logic_error("invalid ast");
        }
        auto right = ast_node_stack.back();
        ast_node_stack.pop_back();
        auto left = ast_node_stack.back();
        ast_node_stack.pop_back();
        ast_node_stack.push_back(std::make_shared<T>(left, right));
    }


    template <typename ParseContext>
    std::shared_ptr<ast_expr> create_ast(ParseContext& pc)
    {
        std::vector<std::shared_ptr<ast_expr>> ast_node_stack;

        for (const auto& match : pc.matches)
        {
            if (match.tag == "num")
            {
                ast_node_stack.push_back(std::make_shared<ast_num>(std::stod(match)));
            }

            else if (match.tag == "div")
            {
                create_binary_ast<ast_div>(ast_node_stack);
            }

            else if (match.tag == "mul")
            {
                create_binary_ast<ast_mul>(ast_node_stack);
            }

            else if (match.tag == "sub")
            {
                create_binary_ast<ast_sub>(ast_node_stack);
            }

            else if (match.tag == "add")
            {
                create_binary_ast<ast_add>(ast_node_stack);
            }
        }

        if (ast_node_stack.size() != 1)
        {
            throw std::logic_error("invalid ast");
        }

        return std::move(ast_node_stack.front());
    }

