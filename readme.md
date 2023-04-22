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

[Parsing Whitespace](#parsing-whitespace)

[Counting Lines And Columns](#counting-lines-and-columns)

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
//declare a grammar
const auto grammar = (-terminalSet('+', '-') >> terminalRange('0', '9')) == std::string("int");

//declare an input
std::string input = "123";

//declare a parse context over the input
ParseContext<> pc(input);

//parse
const bool ok = grammar(pc);

//iterate over recognized matches
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
//whitespace
const auto whitespace = terminal(' ');

//integer
const auto integer = terminalRange('0', '9');

//forward declaration of recursive rule
extern Rule<> values;

//value; it is recursive
const auto value = integer 
                 | terminal('(') >> values >> terminal(')');

//rule
Rule<> values = value >> whitespace >> values;
```

## Left Recursion

The library can parse left recursive grammars.

```cpp
//the recursive rule
extern Rule<> expression;

//and integer is a series of digits
const auto integer = +terminalRange('0', '9');

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

The default element that the parser operates on is a `char`; the default container type that the parser operates on is `std::string`. The default type for match ids is also `std::string`.

These can be changed via template parameters of the class `ParseContext`. The full declararation of it is as follows:

```cpp
template <class SourceType, class MatchIdType, class WSParserType> class ParseContext;
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
const auto c = terminal('B') == C;
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

## Parsing Whitespace

The library offers two ways to parse whitespace:

- by writing a parser that parses whitespace and putting it inside the grammar.
- by providing a whitespace parser as a parameter in ParseContext.

Here is an example of a whitespace parser within a grammar:

```cpp
const auto grammar = 'a' >> cwhitespace() >> 'b' >> cwhitespace() >> 'c';
```

Here is an example of a whitespace parser within a parse context:

```cpp
using MyParseContext = ParseContext<std::string, std::string, CWhitespaceParser>;
const auto grammar = terminal('a') >> 'b' >> 'c';
```

The advantage of putting the whitespace into the parse context is that it simplifies grammar writing.

By default, a parse context uses an empty parser for whitespace, so as that there is no runtime overhead.

## Counting Lines And Columns

Most of the time, a parser needs to report the exact line and column an object exists at or an error happens at. In order to count lines and columns, the library provides a few classes:

- the class `SourceView<T, NL, It>` allows:
	- the creation of a view over source container `T`.
	- the specification of newline traits class `NL`; by default, it is the class `NewlineTraits`.
	- the specification of the iterator class `It` that counts newlines when iterating; by default, it is the class `SourceViewIterator<typename T::const_iterator, NL>`.
- the class `SourceViewIterator<It, NL>` counts lines and columns when the `operator ++()` is applied onto it.
- the class `NewlineTraits` which allows line skipping of the character `\n`.

**The line and column counters start from 1 and not from 0, since that is how most editors count lines and columns.**

Here is an example of how to use the above-mentioned classes to count lines and columns:

```cpp
//using a SourceView over an std::string
using SourceT = SourceView<>;

//using a ParseContext defined over the SourceView defined above;
//also using std::isspace for recognizing whitespace, in order to count newlines based on '\n'.
using ParseContextT = ParseContext<SourceT, std::string, CWhitespaceParser>;

//define a simple grammar 'abc'.
const auto a = terminal('a') == "a";
const auto b = terminal('b') == "b";
const auto c = terminal('c') == "c";
const auto grammar = a >> b >> c;

//the source; a newline is inserted between 'b' and 'c'.
const std::string input = "ab\nc";

//define a source view over the source.
const SourceT sourceView(input);

//define a parse context in order to parse.
ParseContextT pc(sourceView);

//parse
const bool ok = grammar(pc);

assert(ok);
assert(pc.sourceEnded());
assert(pc.matches().size() == 3);

//'a' exists on line 1, column 1.
assert(pc.matches()[0].begin().line() == 1 && pc.matches()[0].begin().column() == 1);

//'b' exists on line 1, column 2.
assert(pc.matches()[1].begin().line() == 1 && pc.matches()[1].begin().column() == 2);

//'c' exists on line 2, column 1.
assert(pc.matches()[2].begin().line() == 2 && pc.matches()[2].begin().column() == 1);
```

### Customizing Newline Traits

If `\n` is not what the newline sequence is, then it can be customized. 

Here is the above example, with a customized newline traits class that recognizes `\r\n` as the newline sequence:

```cpp
class CustomNewlineTraits {
public:
    template <class It> bool operator ()(const It& it, const It& end) const {
        return it[0] == '\r' && it[1] == '\n';
    }

    template <class It> void skip(It& it, const It& end) const {
        it += 2;
    }
};

using SourceT = SourceView<std::string, CustomNewlineTraits>;
using ParseContextT = ParseContext<SourceT, std::string, CWhitespaceParser>;

const auto a = terminal('a') == "a";
const auto b = terminal('b') == "b";
const auto c = terminal('c') == "c";
const auto grammar = a >> b >> c;

const std::string input = "ab\r\nc";
const SourceT sourceView(input);
ParseContextT pc(sourceView);

const bool ok = grammar(pc);

assert(ok);
assert(pc.sourceEnded());
assert(pc.matches().size() == 3);
assert(pc.matches()[0].begin().line() == 1 && pc.matches()[0].begin().column() == 1);
assert(pc.matches()[1].begin().line() == 1 && pc.matches()[1].begin().column() == 2);
assert(pc.matches()[2].begin().line() == 2 && pc.matches()[2].begin().column() == 1);
```

### The ParseContextLC Class

Since the above may be a little complex, the clatss `ParseContextLC` combines a `SourceView` instance and a `ParseContext' instance. Its type signature is the following:

```cpp
    template <class SourceType, class MatchIdType, class WSParserType, class NLTraits> class ParseContextLC;
```

This class allows specifying a source type, a match id type, a whitespace parser type, and a newline traits type.

Here is an example on how to use it:

```cpp
using ParseContextT = ParseContextLC<std::string, std::string, CWhitespaceParser>;

const auto a = terminal('a') == "a";
const auto b = terminal('b') == "b";
const auto c = terminal('c') == "c";
const auto grammar = a >> b >> c;

const std::string input = "ab\nc";
ParseContextT pc(input);

const bool ok = grammar(pc);

assert(ok);
assert(pc.sourceEnded());
assert(pc.matches().size() == 3);
assert(pc.matches()[0].begin().line() == 1 && pc.matches()[0].begin().column() == 1);
assert(pc.matches()[1].begin().line() == 1 && pc.matches()[1].begin().column() == 2);
assert(pc.matches()[2].begin().line() == 2 && pc.matches()[2].begin().column() == 1);
```

