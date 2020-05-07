#include <iostream>
#include "parserlib.hpp"

using namespace std;
using namespace parserlib;


namespace calculator_example {


/******************************************************************************
    GRAMMAR
 ******************************************************************************/


//forward reference of rules
extern Rule<> expr, add, mul;


//number is a list of one or more digits
Rule<> num{ "num", +range('0', '9') };


//value is either a parenthesized expression or a number
Rule<> val{ "val", '(' >> expr >> ')' 
                 | num };


//multiplication operation
Rule<> mul_op{ "mul_op", mul >> '*' >> val };


//division operation
Rule<> div_op{ "div_op", mul >> '/' >> val };


//multiplication rule
Rule<> mul{"mul", mul_op 
                | div_op 
                | val };


//addition operation
Rule<> add_op{ "add_op", add >> '+' >> mul };


//subtraction operation
Rule<> sub_op{ "sub_op", add >> '-' >> mul };


//addition rule
Rule<> add{ "add", add_op 
                 | sub_op 
                 | mul };


//expression
Rule<> expr{ "expr", add };


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
        m_right = std::dynamic_pointer_cast<Expr>(ans.back());
        ans.pop_back();
        m_left = std::dynamic_pointer_cast<Expr>(ans.back());
        ans.pop_back();
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
    GRAMMAR<->AST
 ******************************************************************************/


//rule 'num' creates a 'Number' object
static AST<Number> ast_num(num);


//rule 'mul_op' creates a 'Mul' object
static AST<Mul> ast_mul(mul_op);


//rule 'div_op' creates a 'Div' object
static AST<Div> ast_div(div_op);


//rule 'add_op' creates an 'Add' object
static AST<Add> ast_add(add_op);


//rule 'sub_op' creates a 'Sub' object
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
        ExprPtr root = createAST<Expr>(parseContext);
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


} //namespace calculator_example


void runCalculatorExample()
{
    cout << "Calculator example - start\n";
    calculator_example::tests();
    cout << "Calculator example - end\n\n";
}
