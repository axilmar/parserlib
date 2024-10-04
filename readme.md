# Parserlib

A c++17 recursive-descent parser library that can parse left-recursive grammars.

## Versions
 
 
  - 1.0.0.3
 
 	- Reorganized the library in order to support compiler front ends into a separate namespace. The main library is now in `namespace parserlib::core`.
 	- Added `namespace parserlib::cfe` which now contains the compiler-front-end functionality.
 	- separated tokenization and parsing phases for compiler-front-ends ([example](#Parsing)).
 	- Added relevant documentation and unit tests.
 

  - 1.0.0.2
 
 	Rewrote the library from scratch in order to provide a better interface. Changes:
 	
	- All the getter methods now start with 'get', in order to play better with Intellisense.
	- The `ParseContext` class is now configured over the Source type, with the default class being the class `SourceString`.
	- The class `SourceString` provides custom iterator which counts lines and columns, compatible with the `std::string` interface.
	- The functions `terminal`, `terminalSet`, `terminalRange` are changed to `term`, `oneOf`, `oneIn`.
	- Matches are now only hierarchical (as in `operator >=` of previous version).
	- The `'operator >=` has been replaced with `operator ->*`, which is much more distinct than the former; no more typing accidentally '>>' where `>=` was intended.
	- The default match id type is no longer a string; it is an int.
	- Simplified the left recursion parsing implementation.
 
  - 1.0.0.1
 
  	Added support for compiler front-end construction.
  	
- 1.0.0.0
 
  	Initial release.

## Table Of Contents
[Introduction](#introduction)

[Using the Library](#using-the-library)

[Writing a Grammar](#writing-a-grammar)

[Invoking a Parser](#invoking-a-parser)

[Non-Left Recursion](#non-left-recursion)

[Left Recursion](#left-recursion)

[Customizing a Parser](#customizing-a-parser)

[Matches](#matches)

[Resuming From Errors](#resuming-from-errors)

[Creating Compiler Front-Ends](#creating-compiler-front-ends)

## Introduction

Parserlib allows writing of recursive-descent parsers in c++ using the language's operators in order to imitate <a src="https://en.wikipedia.org/wiki/Extended_Backus%E2%80%93Naur_form">Extended Backus-Naur Form (EBNF)</a> syntax.

The library can handle left recursion.

Here is a Calculator grammar example:

```cpp
extern Rule<> add;

const auto val = +oneIn('0', '9');

const auto num = val
               | '(' >> add >> ')';

Rule<> mul = mul >> '*' >> num
           | mul >> '/' >> num
           | num;

Rule<> add = add >> '+' >> mul
           | add >> '-' >> mul
           | mul;
```

*(NOTE: Rule 'add' needs a forward reference because it is mentioned for the very first time in rule 'num'.)*

The above grammar is a direct translation of the following left-recursive EBNF grammar:

```
add = add + mul
    | add - mul
    | mul
    
mul = mul * num
    | mul / num
    | num
    
num = val
    | ( add )
    
val = (0..9)+
```

## Using the Library

The library is available as headers only, since every class is templated.

In order to use it, you have to have the path to its root include folder in your project's include folder list.

Then, you have to either include the root header, like this:

```cpp
#include "parserlib.hpp"
```

Or use the various headers in the subfolder 'parserlib'.

All the code is included in the namespace `parserlib`:

```cpp
using namespace parserlib;
```

## Writing a Grammar

A grammar can be written as a series of parsing expressions, formed by operators and by functions that create parser objects.

### Terminals

The most basic parser is the `TerminalParser`, which is used to parse a terminal. In order to write a terminal expression, the following code must be written:

```cpp
term('x')
term("abc")
```

*The terminals in this library are by default of type `char`, but they can be customized to be anything.*

Other types of terminal parsers are:

```cpp
oneIn('a', 'z') //parses all values between 'a' and 'z'.
oneOf("+-")     //parses '+' or '-'.
```

### Sequences

Terminals can be combined in sequences using the `operator >>`:

```cpp
const auto ab = term('a') >> term('b');
const auto abc = ab >> term('c');
```

In order to parse a sequence successfully, all members of that sequence shall parse successfully.

### Branches

Expressions can have branches:

```cpp
const auto this_or_that = term("this")
                        | term("that");
```

Branches are followed in top-to-bottom fashion.
If a branch fails to parse, then the next branch is selected, until a branch is found or no more branches exist to follow.

### Loops

- The `operator *` parses an expression 0 or more times.
- The `operator +' parses an expression 1 or more times.

```cpp
+(oneIn('0', '9')) //parse a digit 1 or more times.
```

### Optionals

A parser can be made optional by using the `operator -`:

```cpp
-oneOf("+-") >> oneIn('0', '9') //parse a number; the sign is optional.
```

### Conditionals

- The `operator &` allows parsing an expression without consuming any tokens; it returns true if the parsing succeeds, false if it fails. It can be used to test a specific series of tokens before parsing; it is the logical AND operator.
- The `operator !` inverts the result of a parsing expression; it returns true if the expression returns false and vice versa; it is the logical NOT operator.

For example:

```cpp
!oneOf("+-") >> oneIn('0', '9') //parse an integer without a sign.
```

### Matches

- The `operator ->*` allows the assignment of a match id to a production. For example:


```cpp
(-oneOf("+-") >> oneIn('0', '9')) ->* 'i'
```

## Invoking a Parser

In order to invoke a parser, the appropriate `ParseContext` instance must be created.

```cpp
//declare a grammar
const auto grammar = (-oneOf("+-") >> oneIn('0', '9')) ->* 'i';

//declare an input
SourceString input = "123";

//declare a parse context over the input
ParseContext<> pc(input);

//parse
const bool ok = grammar.parse(pc);

//iterate over recognized matches
for(const auto& match : pc.getMatches()) {
    if (match.getId() == 'i') {
        const auto parsedString = match.getSource();
        //process int
    }
}
```

## Non-left Recursion

Rules allow the writing of recursive grammars via `class Rule<ParseContext>`.

```cpp
//whitespace
const auto whitespace = term(' ');

//integer
const auto integer = oneIn('0', '9');

//forward declaration of recursive rule
extern Rule<> values;

//value; it is recursive
const auto value = integer 
                 | term('(') >> values >> term(')');

//rule
Rule<> values = value >> whitespace >> values;
```

## Left Recursion

The library can parse left recursive grammars.

```cpp
//the recursive rule
extern Rule<> expression;

//and integer is a series of digits
const auto integer = +oneIn('0', '9');

//a value is either an integer or a parenthesized expression
Rule<> value = integer 
             | '(' >> expression >> ')';

//multiplication
Rule<> mul = mul >> '*' >> value
           | mul >> '/' >> value
           | value;
           
//addition
Rule<> add = add >> '+' >> mul
           | add >> '-' >> mul
           | mul;
           
//the root rule
Rule<> expression = add;                      
```

## Customizing a Parser

The class ParseContext is a template and has the following signature:

```cpp
template <class MatchId, class Source> class ParseContext;
```

It allows customizing the source type and the match id type.

### Customizing the source type

By default, a ParseContext instance will use a `SourceString` as an input source. But this can be changed to accomodate any STL like container.

For example, the source can be a static array of integers:

```cpp
ParseContext<int, std::array<int, 1000>> pc(input);
```

### Customizing the match id type

The default match id type is `int`, but usually it shall be an enumeration. 

Example:

```cpp
enum class AST {
	Int,
	Add,
	Sub,
	Mul,
	Div
};

ParseContext<AST> pc(input);
```

### Customizing character processing

the class `SourceString` has the following signature:

```cpp
    template <class Source = std::string, class CaseTraits = CaseSensitiveTraits, class NewlineTraits = DefaultNewlineTraits> 
    class SourceString;
```

It can be customized over:

- the source type; by default, it is an `std::string`.
- the case traits; the library provides two classes: `CaseSensitiveTraits` and `CaseInsensitiveTraits`; the default is the class `CaseSensitiveTraits`.
- the new line traits; the default is the class `DefaultNewlineTraits` which recognizes the character `'\n'` as the newline character.

## Matches

The `operator ->*` allows the creation of a match.

A match can have children; as matches are parsed, the matches parsed under other matches are placed as children of those matches.

In the following example, an IP4 address is returned via a tree of matched, with the following structure:

```
IP4_ADDRESS
    HEX_BYTE
        HEX_DIGIT
        HEX_DIGIT
    HEX_BYTE
        HEX_DIGIT
        HEX_DIGIT
    HEX_BYTE
        HEX_DIGIT
        HEX_DIGIT
    HEX_BYTE
        HEX_DIGIT
        HEX_DIGIT
```

Here is the code:

```cpp
enum TYPE {
    ZERO,
    ONE,
    TWO,
    THREE,
    FOUR,
    FIVE,
    SIX,
    SEVEN,
    EIGHT,
    NINE,
    A,
    B,
    C,
    D,
    E,
    F,
    HEX_DIGIT,
    HEX_BYTE,
    IP4_ADDRESS
};

const auto zero  = terminal('0') ->* ZERO ;
const auto one   = terminal('1') ->* ONE  ;
const auto two   = terminal('2') ->* TWO  ;
const auto three = terminal('3') ->* THREE;
const auto four  = terminal('4') ->* FOUR ;
const auto five  = terminal('5') ->* FIVE ;
const auto six   = terminal('6') ->* SIX  ;
const auto seven = terminal('7') ->* SEVEN;
const auto eight = terminal('8') ->* EIGHT;
const auto nine  = terminal('9') ->* NINE ;

const auto a = terminal('A') ->* A;
const auto b = terminal('B') ->* B;
const auto c = terminal('C') ->* C;
const auto d = terminal('D') ->* D;
const auto e = terminal('E') ->* E;
const auto f = terminal('F') ->* F;

const auto hexDigit = (zero | one | two | three | four | five | six | seven | eight | nine | a | b | c | d | f) ->* HEX_DIGIT;

const auto hexByte = (hexDigit >> hexDigit) ->* HEX_BYTE;

const auto ip4Address = (hexByte >> terminal('.') >> hexByte >> terminal('.') >> hexByte >> terminal('.') >> hexByte) ->* IP4_ADDRESS;

const SourceString input = "FF.12.DC.A0";

ParseContext<TYPE> pc(input);

const bool ok = ip4Address.parse(pc);

assert(ok);
assert(pc.getMatches().size() == 1);

const auto& match = pc.getMatches()[0];

std::stringstream stream;
stream << match.getChildren()[0].getChildren()[0].getSource();
stream << match.getChildren()[0].getChildren()[1].getSource();
stream << '.';
stream << match.getChildren()[1].getChildren()[0].getSource();
stream << match.getChildren()[1].getChildren()[1].getSource();
stream << '.';
stream << match.getChildren()[2].getChildren()[0].getSource();
stream << match.getChildren()[2].getChildren()[1].getSource();
stream << '.';
stream << match.getChildren()[3].getChildren()[0].getSource();
stream << match.getChildren()[3].getChildren()[1].getSource();
const std::string output = stream.str();
std::cout << output;
```

The above prints the input, which is the value `FF.12.DC.A0`.

## Resuming From Errors

The `operator ~`, together with the `operator >>` can be used to create a resume point on a sequence.

For example, parsing a statement that ends with ';', the grammar can be:

```cpp
auto statementDeclaration = statement >> ~term(';');
```

In the above grammar, if 'statement' fails to parse, then the parser will try to find the next ';' and continue parsing statements after that, adding the error to the parse context.

## Creating Compiler Front-Ends

### Creating full compiler front-ends
 
The following code demonstrates how to use the `class CFE<TokenID, ASTID, Source>` to create a custom compiler front end (example copied from unit tests):

```cpp
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
```

In order to create a custom compiler front end, one has to do the following:

1. specify the token id type (usually an enum class).
2. specify the ast id type (also usually an enum class).
3. specify the tokenizer grammar.
4. specify the parser grammar.
5. provide a function which uses the specified tokenizer and parser grammars in order to parse an input.

A CFE instance can be used like this:

```cpp
SourceString input = "(2 + (3 - (4 * (10 / 2)))) * (((12 / 2) * 3) - 5)";
CalculatorCFE cfe;
auto [success, tokens, ast, errors] = cfe.parse(input);
assert(CalculatorCFE::evaluate((*ast)[0]) == (2.0 + (3.0 - (4.0 * (10.0 / 2.0)))) * (((12.0 / 2.0) * 3.0) - 5.0));
```

### Using separate steps for tokenization and parsing

#### Tokenization

The library provides a separate tokenizing function with the following signature:

```cpp
    template <class Source, class Grammar, class TokenContainer, class ErrorContainer>
    bool tokenize(Source& input, Grammar&& grammar, TokenContainer& tokens, ErrorContainer& errors);

```

The type `Source` shall be a STL-compatible container. 

The type `TokenContainer` shall be an STL-compatible container of token structures.

The Token type should follow the conventions of `class parserlib::cfe::Token`.

### Parsing

The library provides separate parse functions with the following signatures:

```cpp
template <class TokenContainer, class Grammar, class ASTContainer, class ErrorContainer, class CreateASTFunc>
    bool parse(TokenContainer& input, Grammar&& grammar, ASTContainer& ast, ErrorContainer& errors, CreateASTFunc&& createAST);
    
template <class TokenContainer, class Grammar, class ASTContainer, class ErrorContainer>
    bool parse(TokenContainer& input, Grammar&& grammar, ASTContainer& ast, ErrorContainer& errors);    
```

The first function uses a custom ast-creation function, whereas the second one uses an internal library function that creates a `class AST<ASTID, Source>` instance.

The type `TokenContainer` shall contain the tokens (which must follow the conventions laid out by the class `parserlib::cfe::Token`).

The type `ASTContainer` shall be a container of ast node pointers.

The type 'ErrorContainer' shall be a container of errors. The error class should follow the conventions laid out by the `class parserlib::core::ParseError`.

For example, the following code uses totally custom Token and AST node types (example taken from unit tests):

```cpp
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
```
