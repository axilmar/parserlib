#include <iostream>
#include <regex>
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


//number is a double
auto digit = range('0', '9');
auto sign = oneOf("+-");
auto numPart = +digit >> -('.' >> *digit) | '.' >> +digit;
auto expPart = oneOf("eE") >> -sign >> +digit;
Rule<> num = -sign >> numPart >> -expPart                  == ast<Number>();


//value
Rule<> val = '(' >> expr >> ')' 
           | num;


//multiplication/division
Rule<> mul = mul >> '*' >> val                             == ast<Mul>()
           | mul >> '/' >> val                             == ast<Div>()
           | val;


//addition/subtraction
Rule<> add = add >> '+' >> mul                             == ast<Add>()
           | add >> '-' >> mul                             == ast<Sub>()
           | mul;


//expression
Rule<> expr = add;


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


} //namespace calculator_example2


void runCalculatorExample2Tests()
{
    cout << "Calculator example 2 - start\n";
    calculator_example2::tests();
    cout << "Calculator example 2 - end\n\n";
}

void runCalculator2Demo()
{
    std::cout << "Calculator 2 interactive example.\n";
    for (;;)
    {
        std::cout << "enter expression or press enter to exit: ";
        std::string str;
        std::getline(std::cin, str);
        if (str.empty()) break;
        str = std::regex_replace(str, std::regex("\\s+"), "");
        ParseContext<> parseContext(str);
        calculator_example2::ExprPtr root = parseContext.parse<calculator_example2::Expr>(calculator_example2::expr);
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
    runCalculator2Demo();
    return 0;
}

#endif


