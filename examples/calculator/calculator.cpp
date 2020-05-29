#include <iostream>
#include <regex>
#include "parserlib.hpp"


using namespace std;
using namespace parserlib;


namespace calculator_example {


/******************************************************************************
    GRAMMAR
 ******************************************************************************/


//forward reference of rules
extern Rule<> expr, add, mul;


//number is a double
auto digit = range('0', '9');
auto sign = oneOf("+-");
auto numPart = +digit >> -('.' >> *digit) | '.' >> +digit;
auto expPart = oneOf("eE") >> -sign >> +digit;
Rule<> num = -sign >> numPart >> -expPart;


//value is either a parenthesized expression or a number
Rule<> val = '(' >> expr >> ')' 
           | num;


//multiplication operation
Rule<> mul_op = mul >> '*' >> val;


//division operation
Rule<> div_op = mul >> '/' >> val;


//multiplication rule
Rule<> mul = mul_op 
           | div_op 
           | val;


//addition operation
Rule<> add_op = add >> '+' >> mul;


//subtraction operation
Rule<> sub_op = add >> '-' >> mul;


//addition rule
Rule<> add = add_op 
           | sub_op 
           | mul;


//expression
Rule<> expr = add;


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


static void test(std::string input, const double value)
{
    ParseContext<> parseContext(input);
    ExprPtr root = parseContext.parse<Expr>(expr);

    cout << input << " => ";
    if (root)
    {
        const double result = root->eval();
        if (result == value)
        {
            cout << "SUCCESS; result = " << result << endl;
        }
        else
        {
            cout << "ERROR; result = " << result << ", correct = " << value << endl;
            throw std::runtime_error("Calculator 2 example error");
        }
    }
    else
    {
        cout << parseContext.getRemainingInput() << endl;
    }
}


static void tests()
{
    test("1", 1.0);
    test("1+2", 1.0+2.0);
    test("1+2+3", 1.0+2.0+3.0);
    test("1+2*3", 1.0+2.0*3.0);
    test("1*2+3", 1.0*2.0+3.0);
    test("(1+2)+3", (1.0+2.0)+3.0);
    test("1+(2+3)", 1.0+(2.0+3.0));
    test("((1)+2)+3", ((1.0)+2.0)+3.0);
    test("1+(2+(3))", 1.0+(2.0+(3.0)));
    test("((1+2)+2)+3", ((1.0+2.0)+2.0)+3.0);
    test("1+(2+(3+4))", 1.0+(2.0+(3.0+4.0)));
    test("((1*2)/2)+3", ((1.0*2.0)/2.0)+3.0);
    test("1+(2*(3-4))", 1.0+(2.0*(3.0-4.0)));
}


} //namespace calculator_example


void runCalculatorExampleTests()
{
    cout << "Calculator example - start\n";
    calculator_example::tests();
    cout << "Calculator example - end\n\n";
}

void runCalculatorDemo()
{
    std::cout << "Calculator interactive example.\n";
    for (;;)
    {
        std::cout << "enter expression or press enter to exit: ";
        std::string str;
        std::getline(std::cin, str);
        if (str.empty()) break;
        str = std::regex_replace(str, std::regex("\\s+"), "");
        ParseContext<> parseContext(str);
        calculator_example::ExprPtr root = parseContext.parse<calculator_example::Expr>(calculator_example::expr);
        if (root)
        {
            const double result = root->eval();
            std::cout << "result = " << result << std::endl;
        }
        else
        {
            std::cout << "ERROR: expression could not be parsed: " << parseContext.getRemainingInput() << std::endl;
        }
    }
}


#ifdef DEMO

int main(int argc, char* argv[])
{
    runCalculatorDemo();
    return 0;
}


#endif //DEMO

