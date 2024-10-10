#include <iostream>
#include "parserlib.hpp"
#include "util.hpp"


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
        std::vector<std::shared_ptr<MyAST>> children;

        MyAST(ASTID id, std::vector<Token>::const_iterator start, std::vector<Token>::const_iterator end)
            : ID(id)
        {
        }

        void addChild(const std::shared_ptr<MyAST>& child) {
            children.push_back(child);
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


static void unitTest_EBNF() {
    core::SourceString<> input =
        "(*comment"
        "*)"
        "choice          = a | b | c;"
        "sequence        = a b c;"
        "exclude         = a - b;"
        "zeroOrMore      = a*;"
        "oneOrMore       = a+;"
        "optional        = a?;"
        "logicalNot      = a!;"
        "logicalAnd      = a&;"
        "group           = (a | b | c);"
        "optionalGroup   = [a | b | c];"
        "repetitionGroup = {a | b | c};"
        "%terminalString = \"aaa\";"
        "%terminalRange  = 'a' .. 'z';"
        "%terminalChar   = 'a';"
        "%whitespace     = '\\0' .. ' ';";

    EBNF ebnf;
    auto [success, tokens, astp, errors] = ebnf.parse(input);

    const auto& ast = *astp;

    if (0) {
        size_t index = 0;
        for (const auto& ap : ast) {
            std::cout << index << ") ";
            ap->print(std::cout);
            std::cout << std::endl;
            ++index;
        }
        printf("");
    }

    assert(ast[0] ==
        tree("Rule",
            tree("NonTerminal: choice"),
            tree("Choice",
                tree("NonTerminal: a"),
                tree("NonTerminal: b"),
                tree("NonTerminal: c"))));

    assert(ast[1] ==
        tree("Rule",
            tree("NonTerminal: sequence"),
            tree("Sequence",
                tree("NonTerminal: a"),
                tree("NonTerminal: b"),
                tree("NonTerminal: c"))));

    assert(ast[2] ==
        tree("Rule",
            tree("NonTerminal: exclude"),
            tree("Exclude",
                tree("NonTerminal: a"),
                tree("NonTerminal: b"))));

    assert(ast[3] ==
        tree("Rule",
            tree("NonTerminal: zeroOrMore"),
            tree("ZeroOrMore",
                tree("NonTerminal: a"))));

    assert(ast[4] ==
        tree("Rule",
            tree("NonTerminal: oneOrMore"),
            tree("OneOrMore",
                tree("NonTerminal: a"))));

    assert(ast[5] ==
        tree("Rule",
            tree("NonTerminal: optional"),
            tree("Optional",
                tree("NonTerminal: a"))));

    assert(ast[6] ==
        tree("Rule",
            tree("NonTerminal: logicalNot"),
            tree("LogicalNot",
                tree("NonTerminal: a"))));

    assert(ast[7] ==
        tree("Rule",
            tree("NonTerminal: logicalAnd"),
            tree("LogicalAnd",
                tree("NonTerminal: a"))));

    assert(ast[8] ==
        tree("Rule",
            tree("NonTerminal: group"),
            tree("Group",
                tree("Choice",
                    tree("NonTerminal: a"),
                    tree("NonTerminal: b"),
                    tree("NonTerminal: c")))));

    assert(ast[9] ==
        tree("Rule",
            tree("NonTerminal: optionalGroup"),
            tree("OptionalGroup",
                tree("Choice",
                    tree("NonTerminal: a"),
                    tree("NonTerminal: b"),
                    tree("NonTerminal: c")))));

    assert(ast[10] ==
        tree("Rule",
            tree("NonTerminal: repetitionGroup"),
            tree("RepetitionGroup",
                tree("Choice",
                    tree("NonTerminal: a"),
                    tree("NonTerminal: b"),
                    tree("NonTerminal: c")))));

    assert(ast[11] ==
        tree("Token",
            tree("NonTerminal: terminalString"),
            tree("TerminalString: aaa")));

    assert(ast[12] ==
        tree("Token",
            tree("NonTerminal: terminalRange"),
            tree("TerminalRange",
                tree("TerminalChar: a"),
                tree("TerminalChar: z"))));

    assert(ast[13] ==
        tree("Token",
            tree("NonTerminal: terminalChar"),
            tree("TerminalChar: a")));

    assert(ast[14] ==
        tree("Token",
            tree("NonTerminal: whitespace"),
            tree("TerminalRange",
                tree("TerminalChar: \\0"),
                tree("TerminalChar:  "))));

    assert(success);
}


void parserlib_cfe_unitTests() {
    unitTest_CFE();
    unitTest_customParsing();
    unitTest_EBNF();
}
