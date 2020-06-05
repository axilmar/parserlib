#include <iostream>
#include "parserlib.hpp"


using namespace parserlib;


extern rule<> expr;


auto num = -terminal('-') >> +range('0', '9')    >= "num";


rule<> val = '(' >> expr >> ')'
           | num;


rule<> mul = mul >> '*' >> val                   >= "mul"
           | mul >> '/' >> val                   >= "div"
           | val;


rule<> add = add >> '+' >> mul                   >= "add"
           | add >> '-' >> mul                   >= "sub"
           | mul;


rule<> expr = add;


class ast_expr : public ast_node<>
{
public:
    virtual double eval() const = 0;
};


class ast_num : public ast_expr
{
public:
    template <typename Match> 
    ast_num(const Match& m, ast_node_stack<>& ast_nodes)
        : m_value(std::stod(m))
    {
    }

    virtual double eval() const
    {
        return m_value;
    }

private:
    const double m_value;
};


class ast_binary_expr : public ast_expr
{
public:
    template <typename Match> 
    ast_binary_expr(const Match& m, ast_node_stack<>& ast_nodes)
    {
        m_right = pop_ast_node<ast_expr>(ast_nodes);
        m_left = pop_ast_node<ast_expr>(ast_nodes);
    }

protected:
    std::shared_ptr<ast_expr> m_left, m_right;
};


class ast_div : public ast_binary_expr
{
public:
    using ast_binary_expr::ast_binary_expr;

    virtual double eval() const
    {
        return m_left->eval() / m_right->eval();
    }
};


class ast_mul : public ast_binary_expr
{
public:
    using ast_binary_expr::ast_binary_expr;

    virtual double eval() const
    {
        return m_left->eval() * m_right->eval();
    }
};


class ast_sub : public ast_binary_expr
{
public:
    using ast_binary_expr::ast_binary_expr;

    virtual double eval() const
    {
        return m_left->eval() - m_right->eval();
    }
};


class ast_add : public ast_binary_expr
{
public:
    using ast_binary_expr::ast_binary_expr;

    virtual double eval() const
    {
        return m_left->eval() + m_right->eval();
    }
};


#define STRINGIFY1(S) #S
#define STRINGIFY(S) STRINGIFY1(S)


#define TEST_EXPR 1+2*3


int main(int argc, char* argv[])
{
    const std::string input = STRINGIFY(TEST_EXPR);
    auto pc = parse_context(input);
    const auto res = parse(expr, pc);
    const auto remaining_input = pc.remaining_input();
    
    /*
    std::cout << "result = " << (int)res << std::endl;
    std::cout << "remaining input = " << remaining_input << std::endl;
    for (const auto& match : pc.matches)
    {
        std::cout << match.tag << " => " << match << std::endl;
    }
    */

    match_table<> mt
    { 
        {"num", ast<ast_num>()},
        {"div", ast<ast_div>()},
        {"mul", ast<ast_mul>()},
        {"sub", ast<ast_sub>()},
        {"add", ast<ast_add>()},
    };


    if (res)
    {
        std::shared_ptr<ast_expr> root = std::dynamic_pointer_cast<ast_expr>(mt.create_ast(pc.matches));
        const double result = root->eval();
        std::cout << "result = " << result << std::endl;
        std::cout << "expected = " << (TEST_EXPR) << std::endl;
    }

    system("pause");
    return 0;
}
