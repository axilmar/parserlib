#include "parserlib.hpp"


using namespace parserlib;
using namespace core;
using namespace cfe;


class CalculatorCFE {
public:
    enum class TokenId {
        Number,
        AddOp,
        SubOp,
        MulOp,
        DivOp,
        LeftParen,
        RightParen
    };

    enum class ASTId {
        Number,
        AddExpr,
        SubExpr,
        MulExpr,
        DivExpr
    };

    template <class CharT, class CharTraits>
    friend std::basic_ostream<CharT, CharTraits>& operator << (std::basic_ostream<CharT, CharTraits>& stream, ASTId id) {
        switch (id) {
            case ASTId::Number:
                stream << "Number";
                break;
            case ASTId::AddExpr:
                stream << "Add";
                break;
            case ASTId::SubExpr:
                stream << "Sub";
                break;
            case ASTId::MulExpr:
                stream << "Mul";
                break;
            case ASTId::DivExpr:
                stream << "Div";
                break;
        }
        return stream;
    }

    typedef CFE<TokenId, ASTId> CFE;

    typedef typename CFE::TokenizerRule TokenizerRule;

    typedef typename CFE::ParserRule ParserRule;

    typedef typename CFE::ASTPtr ASTPtr;

    typedef typename CFE::ASTContainer ASTContainer;

    typedef typename CFE::ErrorContainer ErrorContainer;

    typedef typename CFE::Source Source;

    typedef typename CFE::TokenContainerPtr TokenContainerPtr;

    typedef typename CFE::ASTContainerPtr ASTContainerPtr;

    typedef typename CFE::ErrorContainerPtr ErrorContainerPtr;

    CalculatorCFE() {
        /**** tokenizer ****/
        auto ws = oneIn('\0', ' ');
        auto digit = oneIn('0', '9');
        auto sign = oneOf('+', '-');
        auto integer = +digit;
        auto number = (-sign >> integer >> -('.' >> integer))->*TokenId::Number;
        auto addOp = term('+')->*TokenId::AddOp;
        auto subOp = term('-')->*TokenId::SubOp;
        auto mulOp = term('*')->*TokenId::MulOp;
        auto divOp = term('/')->*TokenId::DivOp;
        auto leftParen = term('(')->*TokenId::LeftParen;
        auto rightParen = term(')')->*TokenId::RightParen;
        m_tokenizerGrammar = *(ws | number | addOp | subOp | mulOp | divOp | leftParen | rightParen);

        /**** parser ****/

        auto parenExpr = TokenId::LeftParen >> m_add >> TokenId::RightParen;

        auto num = term(TokenId::Number)->*ASTId::Number;

        auto val
            = parenExpr
            | num;

        m_mul
            = (m_mul >> TokenId::MulOp >> val)->*ASTId::MulExpr
            | (m_mul >> TokenId::DivOp >> val)->*ASTId::DivExpr
            | val;

        m_add
            = (m_add >> TokenId::AddOp >> m_mul)->*ASTId::AddExpr
            | (m_add >> TokenId::SubOp >> m_mul)->*ASTId::SubExpr
            | m_mul;

        m_parserGrammar = m_add;
    }

    std::tuple<bool, TokenContainerPtr, ASTContainerPtr, ErrorContainerPtr> parse(Source& input) {
        return CFE::parse(input, m_tokenizerGrammar, m_parserGrammar);
    }

    static double evaluate(const ASTPtr& ast) {
        switch (ast->getID()) {
            case ASTId::Number: {
                std::stringstream stream;
                stream << ast->getSource();
                double r;
                stream >> r;
                return r;
            }

            case ASTId::AddExpr:
                return evaluate(ast->getChildren()[0]) + evaluate(ast->getChildren()[1]);

            case ASTId::SubExpr:
                return evaluate(ast->getChildren()[0]) - evaluate(ast->getChildren()[1]);

            case ASTId::MulExpr:
                return evaluate(ast->getChildren()[0]) * evaluate(ast->getChildren()[1]);

            case ASTId::DivExpr:
                return evaluate(ast->getChildren()[0]) / evaluate(ast->getChildren()[1]);
        }

        throw std::logic_error("invalid CalculatorCFE ASTId.");
    }

private:
    TokenizerRule m_tokenizerGrammar;
    ParserRule m_add;
    ParserRule m_mul;
    ParserRule m_parserGrammar;
};


static void unitTest_CFE() {
    CalculatorCFE cfe;

    {
        SourceString input = "1";
        auto [success, tokens, ast, errors] = cfe.parse(input);
        assert(CalculatorCFE::evaluate((*ast)[0]) == 1);
    }

    {
        SourceString input = "1.5";
        auto [success, tokens, ast, errors] = cfe.parse(input);
        assert(CalculatorCFE::evaluate((*ast)[0]) == 1.5);
    }

    {
        SourceString input = "-1";
        auto [success, tokens, ast, errors] = cfe.parse(input);
        assert(CalculatorCFE::evaluate((*ast)[0]) == -1);
    }

    {
        SourceString input = "+1";
        auto [success, tokens, ast, errors] = cfe.parse(input);
        assert(CalculatorCFE::evaluate((*ast)[0]) == +1);
    }

    {
        SourceString input = "-1.5";
        auto [success, tokens, ast, errors] = cfe.parse(input);
        assert(CalculatorCFE::evaluate((*ast)[0]) == -1.5);
    }

    {
        SourceString input = "+1.5";
        auto [success, tokens, ast, errors] = cfe.parse(input);
        assert(CalculatorCFE::evaluate((*ast)[0]) == +1.5);
    }

    {
        SourceString input = "1 + 1";
        auto [success, tokens, ast, errors] = cfe.parse(input);
        assert(CalculatorCFE::evaluate((*ast)[0]) == 1.0 + 1.0);
    }

    {
        SourceString input = "2 - 3";
        auto [success, tokens, ast, errors] = cfe.parse(input);
        assert(CalculatorCFE::evaluate((*ast)[0]) == 2.0 - 3.0);
    }

    {
        SourceString input = "2 * 3";
        auto [success, tokens, ast, errors] = cfe.parse(input);
        assert(CalculatorCFE::evaluate((*ast)[0]) == 2.0 * 3.0);
    }

    {
        SourceString input = "2 / 3";
        auto [success, tokens, ast, errors] = cfe.parse(input);
        assert(CalculatorCFE::evaluate((*ast)[0]) == 2.0 / 3.0);
    }

    {
        SourceString input = "(2 + (3 - (4 * (10 / 2)))) * (((12 / 2) * 3) - 5)";
        auto [success, tokens, ast, errors] = cfe.parse(input);
        assert(CalculatorCFE::evaluate((*ast)[0]) == (2.0 + (3.0 - (4.0 * (10.0 / 2.0)))) * (((12.0 / 2.0) * 3.0) - 5.0));
    }
}


static void unitTest_customParsing() {
    enum class TokenKind {
        A,
        B,
        C
    };

    struct Token {
        TokenKind kind;
        std::string lexeme;
        int row;
        int column;

        bool operator == (TokenKind tk) const {
            return kind == tk;
        }
    };

    std::vector<Token> tokens;

    enum class ASTID {
        A,
        B,
        C
    };

    const auto grammar
        = *(TokenKind::A->*ASTID::A
        |   TokenKind::B->*ASTID::B
        |   TokenKind::C->*ASTID::C)
        ;

    class MyAST {
    public:
        typedef ASTID ASTID;

        typedef std::vector<Token> Source;

        ASTID ID;

        MyAST(ASTID id, std::vector<Token>::const_iterator start, std::vector<Token>::const_iterator end, std::vector<std::shared_ptr<MyAST>>&& children)
            : ID(id)
        {
        }
    };

    std::vector<std::shared_ptr<MyAST>> ast;

    class Error {
    public:
        std::vector<Token>::const_iterator START;

        Error(int id, std::vector<Token>::const_iterator start, std::vector<Token>::const_iterator end)
            : START(start)
        {
        }

        bool operator < (const Error& err) const {
            return START < err.START;
        }
    };

    std::vector<Error> errors;

    std::vector<Token> input;
    input.push_back(Token{ TokenKind::C, "", 0, 0 });
    input.push_back(Token{ TokenKind::B, "", 0, 0 });
    input.push_back(Token{ TokenKind::A, "", 0, 0 });

    cfe::parse(input, grammar, ast, errors);

    assert(ast.size() == 3);
    assert(ast[0]->ID == ASTID::C);
    assert(ast[1]->ID == ASTID::B);
    assert(ast[2]->ID == ASTID::A);
}


void parserlib_cfe_unitTests() {
    unitTest_CFE();
    unitTest_customParsing();
}
