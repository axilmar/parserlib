#include <iostream>
#include "parserlib.hpp"

using namespace std;
using namespace parserlib;


namespace calculator_example2 {


/******************************************************************************
    AST
 ******************************************************************************/


//base class for expressions
class Expr : public ASTNode
{
public:
    //evaluate expression
    virtual double eval() const = 0;
};


//pointer to expression
typedef std::shared_ptr<Expr> ExprPtr;


//number
class Number : public Expr
{
public:
    //get number from match
    Number(const Match<>& match, ASTNodeStack& ans)
    {
        m_number = stod(match);
    }

    //return number
    virtual double eval() const
    {
        return m_number;
    }

private:
    double m_number{};
};


//base class for binary expressions
class BinExpr : public Expr
{
public:
    //get left/right expressions from node stack
    BinExpr(const Match<>& match, ASTNodeStack& ans)
    {
        m_right = ans.pop<Expr>();
        m_left = ans.pop<Expr>();
    }

protected:
    ExprPtr m_left, m_right;
};


//multiplication
class Mul : public BinExpr
{
public:
    using BinExpr::BinExpr;

    //multiply left to right
    virtual double eval() const
    {
        return m_left->eval() * m_right->eval();
    }
};


//division
class Div : public BinExpr
{
public:
    using BinExpr::BinExpr;

    //divide left by right
    virtual double eval() const
    {
        return m_left->eval() / m_right->eval();
    }
};


//addition
class Add : public BinExpr
{
public:
    using BinExpr::BinExpr;

    //add left and right
    virtual double eval() const
    {
        return m_left->eval() + m_right->eval();
    }
};


//subtraction
class Sub : public BinExpr
{
public:
    using BinExpr::BinExpr;

    //subtract right from left
    virtual double eval() const
    {
        return m_left->eval() - m_right->eval();
    }
};


/******************************************************************************
    GRAMMAR
 ******************************************************************************/


//forward reference of rules
extern Rule<> expr;


//number
Rule<> num = +range('0', '9')          == ast<Number>();


//value
Rule<> val = '(' >> expr >> ')' 
           | num;


//multiplication/division
Rule<> mul = mul >> '*' >> val         == ast<Mul>()
           | mul >> '/' >> val         == ast<Div>()
           | val;


//addition/subtraction
Rule<> add = add >> '+' >> mul         == ast<Add>()
           | add >> '-' >> mul         == ast<Sub>()
           | mul;


//expression
Rule<> expr = add;


/******************************************************************************
    main
 ******************************************************************************/


static void test(std::string input)
{
    ParseContext<> parseContext(input);
    const auto root = parseContext.parse<Expr>(expr);

    cout << input << " => ";
    if (root)
    {
        cout << "SUCCESS; result = ";
        cout << root->eval() << std::endl;
    }
    else
    {
        cout << parseContext.getRemainingInput() << endl;
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


} //namespace calculator_example2


void runCalculatorExample2()
{
    cout << "Calculator example - start\n";
    calculator_example2::tests();
    cout << "Calculator example - end\n\n";
}
