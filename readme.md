# Parserlib

A c++17 recursive-descent parser library that can parse left-recursive grammars.

## Table Of Contents
[Introduction](#introduction)

[Using the Library](#using-the-library)

[Writing a Grammar](#writing-a-grammar)

[Invoking a Parser](#invoking-a-parser)

[Non-Left Recursion](#non-left-recursion)

[Left Recursion](#left-recursion)

[Customizing a Parser](#customizing-a-parser)

[Simple Matches](#simple-matches)

[Tree Matches](#tree-matches)

## <a id="Introduction"></a>Introduction

Parserlib allows writing of recursive-descent parsers in c++ using the language's operators in order to imitate <a src="https://en.wikipedia.org/wiki/Extended_Backus%E2%80%93Naur_form">Extended Backus-Naur Form (EBNF)</a> syntax.

The library can handle left recursion.

Here is a Calculator grammar example:

```cpp
extern Rule<> add;

const auto val = +terminalRange('0', '9');

const auto num = val
               | '(' >> add >> ')';

Rule<> mul = mul >> '*' >> num
           | mul >> '/' >> num
           | num;

Rule<> add = add >> '+' >> mul
           | add >> '-' >> mul
           | mul;
```

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
terminal('x')
terminal("abc")
```

*The terminals in this library are by default of type `char`, but they can be customized to be anything.*

Other types of terminal parsers are:

```cpp
terminalRange('a', 'z') //parses all values between 'a' and 'z'.
terminalSet('+', '-') //parses '+' or '-'.
```

### Sequences

Terminals can be combined in sequences using the `operator >>`:

```cpp
const auto ab = terminal('a') >> terminal('b');
const auto abc = ab >> terminal('c');
```

In order to parse a sequence successfully, all members of that sequence shall parse successfully.

### Branches

Expressions can have branches:

```cpp
const auto this_or_that = terminal("this")
                        | terminal("that");
```

Branches are followed in top-to-bottom fashion.
If a branch fails to parse, then the next branch is selected, until a branch is found or no more branches exist to follow.

### Loops

- The `operator *` parses an expression 0 or more times.
- The `operator +' parses an expression 1 or more times.

```cpp
+(terminalRange('0', '9')) //parse a digit 1 or more times.
```

### Optionals

A parser can be made optional by using the `operator -`:

```cpp
-terminalSet('+', '-') >> terminalRange('0', '9') //parse a number; the sign is optional.
```

### Conditionals

- The `operator &` allows parsing an expression without consuming any tokens; it returns true if the parsing succeeds, false if it fails. It can be used to test a specific series of tokens before parsing.
- The `operator !` inverts the result of a parsing expression; it returns true if the expression returns false and vice versa.

```cpp
!terminalSet('=', '-') >> terminalRange('0', '9') //parse an integer without a sign.
```

### Matches

- The `operator ==` allows the assignment of a match id to a production; [the created match does not have any children](#simple-matches).
- The `operator >=` allows the assignment of a match id to a production; [the created match has children matches](#tree-matches).


```cpp
(-terminalSet('+', '-') >> terminalRange('0', '9')) == std::string("int")
```

## Invoking a Parser

In order to invoke a parser, the appropriate `ParseContext` instance must be created.

```cpp
const auto grammar = (-terminalSet('+', '-') >> terminalRange('0', '9')) == std::string("int");
std::string input = "123";
ParseContext<> pc(input);
const bool ok = grammar(pc);
for(const auto& match : pc.matches()) {
    if (match.id() == "int") {
        const auto parsedString = match.content();
        //process int
    }
}
```

## Non-left Recursion

Rules allow the writing of recursive grammars.

```cpp
const auto whitespace = terminal(' ');
const auto integer = terminalRange('0', '9');
extern Rule<> values;
const auto value = integer 
                 | terminal('(') >> values >> terminal(')');
Rule<> values = value >> whitespace >> values;
```

The library stores a reference to a rule inside an expression, and therefore rules shall always be used as lvalues.

## Left Recursion

The library can parse direct left recursive grammars.

```cpp
extern Rule<> expression;
const auto integer = +terminalRange('0', '9');
Rule<> value = integer | terminal('(') >> expression >> terminal(')');
Rule<> mul = mul >> terminal('*') >> value
           | mul >> terminal('/') >> value
           | value;
Rule<> add = add >> terminal('+') >> mul
           | add >> terminal('-') >> mul
           | mul;
Rule<> expression = add;                      
```

For recursive grammars, parse expressions must be wrapped into a `Rule<>` instance.

## Customizing a Parser

The default element that the parser operates on is a `char`; the default container type that the parser operates on is `std::string`. The default type for match ids is also `std::string`.

These can be changed via template parameters of the class `ParseContext`. The full declararation of it is as follows:

```cpp
template <class SourceType, class MatchIdType> class ParseContext;
```

For example, in order to iterate over an array of enum values, with match id also being an enum value, the following can be written:

```cpp
enum TOKEN_TYPE {
    TOKEN_INT,
    TOKEN_IDENTIFIER,
    TOKEN_STRING,
    TOKEN_TYPEDEF
};

enum MATCH_ID_TYPE {
    MATCH_VARIABLE,
    MATCH_FUNCTION,
    MATCH_TYPEDEF
};

using ParseContext = parserlib::ParseContext<std::vector<TOKEN_TYPE>, MATCH_ID_TYPE>;
using Rule = parserlib::Rule<ParseContext>;
```

The library does not care about the source type and the match id type, they can be anything. The source type should follow STL container conventions.

## Simple Matches

The `operator ==` allows the creation of a match, when an expression parses successfully. The right hand side should be an expression which evaluates to the match id expected by the parse context. Example:

```cpp
enum TYPE {
    A, B, C
};

const auto a = terminal('A') == A;
const auto b = terminal('B') == B;
const auto c = terminal('B') == B;
const auto grammar = a >> b >> c;

std::string input = "ABC";
ParseContext<std::string, Type> pc(input);

const bool ok = grammar(pc);
for(const auto& match : pc.matches()) {
    std::cout << match.content() << " = " << match.id() << std::endl;
}
```

The above produces the output:

```
A = 0
B = 1
C = 2
```

## Tree Matches

The `operator >=` allows the creation of a match, like the `operator ==`, with a difference: all matches created within the context of the expression are placed as children matches.

This allows matches to also be trees, instead of a flat list. 

In the following example, an IP4 address is returned as a tree match, with the following structure:

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

const auto zero  = terminal('0') == ZERO ;
const auto one   = terminal('1') == ONE  ;
const auto two   = terminal('2') == TWO  ;
const auto three = terminal('3') == THREE;
const auto four  = terminal('4') == FOUR ;
const auto five  = terminal('5') == FIVE ;
const auto six   = terminal('6') == SIX  ;
const auto seven = terminal('7') == SEVEN;
const auto eight = terminal('8') == EIGHT;
const auto nine  = terminal('9') == NINE ;

const auto a = terminal('A') == A;
const auto b = terminal('B') == B;
const auto c = terminal('C') == C;
const auto d = terminal('D') == D;
const auto e = terminal('E') == E;
const auto f = terminal('F') == F;

const auto hexDigit = (zero | one | two | three | four | five | six | seven | eight | nine | a | b | c | d | f) >= HEX_DIGIT;

const auto hexByte = (hexDigit >> hexDigit) >= HEX_BYTE;

const auto ip4Address = (hexByte >> terminal('.') >> hexByte >> terminal('.') >> hexByte >> terminal('.') >> hexByte) >= IP4_ADDRESS;

const std::string input = "FF.12.DC.A0";

ParseContext<std::string, TYPE> pc(input);
using Match = typename ParseContext<std::string, TYPE>::Match;

const bool ok = ip4Address(pc);

assert(ok);
assert(pc.matches().size() == 1);

const Match& match = pc.matches()[0];

std::stringstream stream;
stream << match.children()[0].children()[0].content();
stream << match.children()[0].children()[1].content();
stream << '.';
stream << match.children()[1].children()[0].content();
stream << match.children()[1].children()[1].content();
stream << '.';
stream << match.children()[2].children()[0].content();
stream << match.children()[2].children()[1].content();
stream << '.';
stream << match.children()[3].children()[0].content();
stream << match.children()[3].children()[1].content();
const std::string output = stream.str();
std::cout << output;
```

The above prints the input, which is the value `FF.12.DC.A0`.
