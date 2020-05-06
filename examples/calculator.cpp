#include <iostream>
#include "parserlib.hpp"

using namespace std;
using namespace parserlib;


/******************************************************************************
    GRAMMAR
 ******************************************************************************/


extern Rule<> expr, add, mul;
Rule<> num   { "num"   , +range('0', '9') };
Rule<> val   { "val"   , '(' >> expr >> ')' | num };
Rule<> mul_op{ "mul_op", mul >> '*' >> val };
Rule<> div_op{ "div_op", mul >> '/' >> val };
Rule<> mul   { "mul"   , mul_op | div_op | val };
Rule<> add_op{ "add_op", add >> '+' >> mul };
Rule<> sub_op{ "sub_op", add >> '-' >> mul };
Rule<> add   { "add"   , add_op | sub_op | mul };
Rule<> expr  { "expr"  , add };


/******************************************************************************
    AST
 ******************************************************************************/


class Expr : public ASTNode
{
public:
    virtual double eval() const = 0;
};


typedef std::shared_ptr<Expr> ExprPtr;


class Number : public Expr
{
public:
    Number(const Match<>& match, ASTNodeStack& ans)
    {
        m_number = stod(match);
    }

    virtual double eval() const
    {
        return m_number;
    }

private:
    double m_number{};
};


class BinExpr : public Expr
{
public:
    BinExpr(const Match<>& match, ASTNodeStack& ans)
    {
        m_right = std::dynamic_pointer_cast<Expr>(ans.back());
        ans.pop_back();
        m_left = std::dynamic_pointer_cast<Expr>(ans.back());
        ans.pop_back();
    }

protected:
    ExprPtr m_left, m_right;
};


class Mul : public BinExpr
{
public:
    using BinExpr::BinExpr;

    virtual double eval() const
    {
        return m_left->eval() * m_right->eval();
    }
};


class Div : public BinExpr
{
public:
    using BinExpr::BinExpr;

    virtual double eval() const
    {
        return m_left->eval() / m_right->eval();
    }
};


class Add : public BinExpr
{
public:
    using BinExpr::BinExpr;

    virtual double eval() const
    {
        return m_left->eval() + m_right->eval();
    }
};


class Sub : public BinExpr
{
public:
    using BinExpr::BinExpr;

    virtual double eval() const
    {
        return m_left->eval() - m_right->eval();
    }
};


/******************************************************************************
    GRAMMAR<->AST
 ******************************************************************************/


static AST<Number> ast_num(num);
static AST<Mul> ast_mul(mul_op);
static AST<Div> ast_div(div_op);
static AST<Add> ast_add(add_op);
static AST<Sub> ast_sub(sub_op);


/******************************************************************************
    main
 ******************************************************************************/


static void test(std::string input)
{
    ParseContext<> parseContext(input);
    const bool ok = expr.parse(parseContext);
    const auto result = std::string(parseContext.getCurrentPosition(), parseContext.getEndPosition());

    cout << input << " => ";
    if (result.empty())
    {
        cout << "SUCCESS; result = ";
        ExprPtr root = std::dynamic_pointer_cast<Expr>(createAST(parseContext));
        cout << root->eval() << std::endl;
    }
    else
    {
        cout << result << endl;
    }
}


static void tests()
{
    test("1");
    test("1+2");
    test("1+2+3");
    test("1+2*3");
    test("1*2+3");
    test("(1+2)+3");
    test("1+(2+3)");
    test("((1)+2)+3");
    test("1+(2+(3))");
    test("((1+2)+2)+3");
    test("1+(2+(3+4))");
    test("((1*2)/2)+3");
    test("1+(2*(3-4))");
}


void runCalculatorExample()
{
    cout << "Calculator example - start\n";
    tests();
    cout << "Calculator example - end\n";
}
