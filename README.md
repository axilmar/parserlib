# Parserlib

A c++17 Parsing Expression Grammar (PEG) recursive-descent library.

**Current Version**

1.0.0.12

**Features**

- PEG parser with EBNF-like syntax.
- left-associative parsing.
- can be used as a lexer and parser.
- supports case insensitive parsing.
- supports line and column.
- can be used on STL containers and streams.

**Quick Example: Calculator**

```cpp
#include "parserlib.hpp"
using namespace parserlib;

using p = parser<>;

extern p::rule add;

//digit
auto digit 
	= p::range('0', '9')
    ;

//number
auto number
	= +digit >> -('.' >> +digit)
    ;

//value
auto value
	= '(' >> add >> ')'
    | number
    ;

//multiplication/division
p::rule mul
	= p::left_associative(
    	value,
        '*' >> value,
    	'/' >> value)
    ;

//addition/subtraction
p::rule add
	= p::left_associative(
    	mul,
        '+' >> mul,
    	'-' >> mul)
    ;
```

## Using the library

There is only one header to copy and use in your projects (file `include/parserlib.hpp`).

```cpp
#include "parserlib.hpp"
using namespace parserlib;
```

Everything lives in namespace `parserlib`. The library requires a c++17 compiler and has no other dependencies beyond the standard library.

## Table of contents

- [Core idea](#core-idea)
- [The `parser` class template](#the-parser-class-template)
- [Terminals](#terminals)
- [Combinators](#combinators)
- [Rules (recursive grammars)](#rules-recursive-grammars)
- [Parsing a source and reading the result](#parsing-a-source-and-reading-the-result)
- [Matches](#matches)
- [Left-associative parsing](#left-associative-parsing)
- [Error handling](#error-handling)
- [Line and column tracking](#line-and-column-tracking)
- [Custom parsing with functions](#custom-parsing-with-functions)
- [Using the library as a lexer and a parser](#using-the-library-as-a-lexer-and-a-parser)
- [Case-insensitive / custom symbol comparison](#case-insensitive--custom-symbol-comparison)
- [Debugging a grammar](#debugging-a-grammar)
- [Full example: a calculator](#full-example-a-calculator)
- [Examples and tests](#examples-and-tests)

## Core idea

A grammar is written directly as a C++ expression, built out of small parsing building blocks (terminals) combined with operators (`>>` for sequence, `|` for choice, etc.) that mirror EBNF. There is no code generation step and no separate grammar file — the grammar *is* C++ code, and it is a normal PEG: alternatives (`|`) are tried in order and the first one that matches wins (no ambiguity, no backtracking across the whole grammar the way a CFG parser would need).

Parsing produces a tree of **matches** on demand — you decide which sub-expressions are worth keeping as named nodes (with `->*id`), and the library builds a tree of `match_instance` objects out of exactly those. There is no separate/implicit AST type to learn.

## The `parser` class template

Everything is scoped under one class template:

```cpp
template <
    class Iterator = std::string::const_iterator,
    class MatchId = int,
    class ErrorId = int,
    class SymbolComparator = default_symbol_comparator
>
class parser;
```

- `Iterator` — the iterator type over the source being parsed. Defaults to `std::string::const_iterator`, but can be any forward iterator: a `std::vector<T>::const_iterator`, a `std::istreambuf_iterator`, or the library's own [`text_position_iterator`](#line-and-column-tracking) which additionally tracks line/column.
- `MatchId` — the type used to tag matches (typically an `enum`/`enum class`). Defaults to `int`.
- `ErrorId` — the type used to tag errors (typically an `enum`/`enum class`). Defaults to `int`.
- `SymbolComparator` — a policy class with a static `compare(left, right)` function used every time two symbols are compared; see [Case-insensitive / custom symbol comparison](#case-insensitive--custom-symbol-comparison).

You typically alias the instantiation you need once per grammar:

```cpp
using p = parser<>;                                 //parses a std::string, ints for ids
using p = parser<std::string::const_iterator, MY_TOKEN_ID>;   //custom match id enum
using p = parser<text_position_iterator<>>;          //track line/column
```

Everything else described below — `rule`, `parse_context`, `match_instance`, `term()`, operators, etc. — is a nested member of this class (accessed as `p::rule`, `p::parse_context`, ...).

## Terminals

| Expression | Matches |
|---|---|
| `p::term('a')` or just `'a'`* | a single symbol equal to `'a'` |
| `p::term("abc")` or just `"abc"`* | the literal sequence of symbols `"abc"` |
| `p::set("abc")` | a single symbol that is any one of `'a'`, `'b'`, `'c'` (binary search, fast for big sets) |
| `p::range('0', '9')` | a single symbol within the inclusive range `['0', '9']` |
| `p::any()` | any single symbol (fails only at end of input) |
| `p::end()` | succeeds only at the end of input, without consuming anything |

\* A raw character/string literal is auto-converted to a terminal parse node wherever the DSL expects one (thanks to `parse_node_ptr`'s implicit constructors), so you rarely need to write `p::term(...)` explicitly except as the first symbol of an expression (C++ needs *something* with the right operators to kick off the expression, e.g. `p::term('+') | '-'`).

Terminals aren't limited to `char`: since the parser is templated on the source iterator, you can parse over a `std::vector<MyToken>` and use `p::term(MyToken::SOME_KIND)` etc. — this is exactly how the library is used as a two-stage lexer + parser (see below).

## Combinators

Given parse expressions `a`, `b`, ...:

| Operator | Meaning |
|---|---|
| `a >> b` | sequence: `a` then `b` |
| `a \| b` | ordered choice: `a`, or if it fails, `b` |
| `-a` | optional: zero or one `a` (always succeeds) |
| `*a` | zero or more `a` |
| `+a` | one or more `a` |
| `a * n` | exactly `n` repetitions of `a` |
| `&a` | positive lookahead: succeeds if `a` matches, consumes nothing |
| `!a` | negative lookahead: succeeds if `a` does *not* match, consumes nothing |
| `a - b` | `a`, as long as `b` does not match at that position (`!b >> a`) — handy for "any character except ..." |

Sequences and choices flatten automatically, so `a >> b >> c` and `a \| b \| c` build one n-ary node each rather than nesting pairwise.

Example (excerpt from `examples/xml.cpp`):

```cpp
auto string = '"' >> *(p::any() - '"') >> '"';   //a quoted string: any char except '"'
```

## Rules (recursive grammars)

Plain `auto`-deduced expressions are fine for non-recursive grammar fragments, but a grammar that refers to itself (directly or through other rules) needs a `p::rule`: a named placeholder that can be forward-declared, assigned to later, and safely referenced before its definition is complete.

```cpp
extern p::rule expression;               //forward declaration

auto value
    = number
    | '(' >> expression >> ')'           //expression is not yet defined here — that's fine
    ;

p::rule expression = value;              //definition
```

A `rule` can be constructed from anything a `parse_node_ptr` can: a symbol, a string, a function, or another parse expression. Rules can also be used to keep large grammars readable by naming intermediate expressions, even where recursion isn't strictly required.

## Parsing a source and reading the result

```cpp
std::string input = "1+2*3";
p::parse_context pc{ input };            //also accepts a pair of iterators: { begin, end }
bool ok = grammar.parse(pc);

if (ok) {
    for (const auto& match : pc.get_matches()) {
        // ...
    }
}
for (const auto& error : pc.get_errors()) {
    // ...
}
```

- `parse_context` holds the current parse position, the (possibly nested) list of top-level matches produced so far, and any recorded errors. Construct it from a `{begin, end}` iterator pair or directly from a container (`std::string`, `std::vector<T>`, ...).
- `grammar.parse(pc)` returns `true`/`false`. On failure, the parse position is left where the outermost failing alternative started (backtracking is automatic and local to each combinator).
- To make sure the whole input was consumed, add `p::end()` to the end of your top-level rule: `auto grammar = expression >> p::end();`.

## Matches

A parsed sub-expression only becomes part of the result tree if you say so, with `->*`:

```cpp
auto number = (+digit) ->* NUM;   //tags this sub-match with id NUM
```

This wraps the sub-expression so that, if it matches, a `match_instance` is added to the parse context — nested inside whatever match is currently being built. Each `match_instance` (found in `pc.get_matches()`, or in `.get_children()` of a parent match) exposes:

- `get_id()` — the `MatchId` given to `->*`.
- `get_source()` — the matched span as a `std::basic_string`/`std::vector` (whichever fits the iterator's value type).
- `get_origin_source()` — like `get_source()`, but if this parse ran over *another* parse's matches (see the lexer/parser pattern below), it walks back to the original source text.
- `get_children()` / `get_matches()` — the nested matches captured while parsing this one.
- `begin()` / `end()` — iterators delimiting the match.

```cpp
// walking a match tree, e.g. to evaluate a calculator expression:
double eval(const p::match_instance& m) {
    switch (m.get_id()) {
        case NUM: /* parse m.get_source() as a number */;
        case ADD: return eval(m.get_children()[0]) + eval(m.get_children()[1]);
        // ...
    }
}
```

For fine-grained control over exactly which iterator range a match spans (e.g. spanning several already-matched pieces into one bigger match — this is what `left_associative` uses internally), three lower-level primitives are also available: `p::begin_match()` marks the start position, `p::add_match(id)` (a.k.a. `p::match(id)`) closes a match from that position to the current position, and `p::end_match()` releases the marked position. `a ->* id` is really just sugar for `begin_match() >> a >> add_match(id) >> end_match()`.

## Left-associative parsing

Classic recursive-descent PEG can't express left-recursive rules like `add = add '+' mul | mul` directly (it would recurse forever). Parserlib provides `left_associative` instead, which gets the same effect — proper left-to-right associativity — without recursion or exceptions:

```cpp
// EBNF:  add = add '+' mul | add '-' mul | mul ;
auto add = p::left_associative(
    mul,                            // base
    '+' >> mul >> p::match(ADD),    // branch: consumes '+' mul, folds into a left-associative ADD match
    '-' >> mul >> p::match(SUB)
);
```

This expands to `begin_match() >> mul >> *('+' >> mul >> match(ADD) | '-' >> mul >> match(SUB)) >> end_match()`: it parses one `mul`, then repeatedly parses a `+`/`-` branch, each time folding the previously accumulated match together with the new right-hand side into a fresh match that starts from the very first position. The result is a properly left-nested match tree (`((1+2)-3)` rather than `(1+(2-3))`) with no recursion involved.

## Error handling

`p::error(skip_node, error_id)` records an error and lets parsing continue rather than aborting the whole grammar. It's normally paired with `p::skip_before(node)` / `p::skip_after(node)`, which advance the parse position until `node` matches (without recording any matches while skipping), stopping either just before or just after it:

```cpp
using p = parser<text_position_iterator<>>;

// on each ';'-terminated item: either "a;" is parsed, or everything up to
// and including the next ';' is skipped and recorded as an error.
auto grammar = *(p::term('a') >> ';' | p::error(p::skip_after(';'), SYNTAX_ERROR));

p::parse_context pc{ input };
grammar.parse(pc);
for (const auto& err : pc.get_errors()) {
    std::cout << "error at line " << err.begin().get_line()
               << ", column "     << err.begin().get_column() << "\n";
}
```

This is the same pattern `examples/json.cpp` uses in its tokenizer stage to recover from unrecognized characters instead of failing the whole tokenization.

## Line and column tracking

Swap in `text_position_iterator<It>` (default `It = std::string::const_iterator`) as the parser's `Iterator` type to get 1-based line/column tracking on every match and error:

```cpp
using p = parser<text_position_iterator<>>;

auto grammar = *(letter ->* IDENTIFIER | p::newline(p::term('\n')));

p::parse_context pc{ input };
grammar.parse(pc);
std::cout << pc.get_matches()[0].begin().get_line() << ":"
          << pc.get_matches()[0].begin().get_column();
```

Column advances automatically as symbols are consumed; wrap whatever your grammar treats as a newline with `p::newline(...)` so the line counter advances too (the library can't infer what counts as a "newline" for you, since sources aren't always `char` text).

## Custom parsing with functions

Any callable with the signature `bool(parse_context&)` can be dropped directly into a grammar expression — useful for parsing logic that's awkward to express with the built-in combinators, or for side effects (semantic actions, symbol-table lookups, etc.) hooked in mid-parse:

```cpp
auto custom = p::function([](p::parse_context& pc) {
    if (pc.is_valid_iterator() && /* some check */) {
        pc.increment_iterator();
        return true;
    }
    return false;
});
```

A lambda used directly in an expression (e.g. `a >> [](auto& pc){ ... } >> b`) is picked up automatically too, without needing `p::function(...)`.

## Using the library as a lexer and a parser

Because `parser<Iterator, ...>` is templated on the iterator type, you can run the library twice: once over raw text to produce a flat/nested list of `match_instance` tokens, and again treating `std::vector<match_instance>::const_iterator` as the new source, building a second grammar whose terminals are the token ids from the first pass. `examples/json.cpp` does exactly this:

```cpp
// --- stage 1: tokenizer over text ---
using t = parserlib::parser<std::string::const_iterator, TOKEN_ID, TOKEN_ERROR_ID>;
auto tokenizer = *token_element >> t::end();

t::parse_context tokenizer_pc{ input };
tokenizer.parse(tokenizer_pc);

// --- stage 2: parser over the token stream ---
using parser_input = std::vector<t::match_instance>;
using p = parserlib::parser<parser_input::const_iterator, PARSER_ID>;

p::parse_context parser_pc{ tokenizer_pc.get_matches() };
element.parse(parser_pc);   // 'element' is written in terms of p::term(TOKEN_ID::...)
```

In the second stage, `match_instance::get_origin_source()` walks back through the token match to the *original* text span it came from, which is handy for reconstructing/echoing exact source text from the final match tree.

## Case-insensitive / custom symbol comparison

Symbol comparisons all go through `SymbolComparator::compare(left, right)` (the fourth template parameter of `parser<>`), so case-insensitive matching (or any other custom equivalence) is a matter of supplying your own comparator:

```cpp
struct case_insensitive_comparator {
    template <class L, class R>
    static intptr_t compare(const L& left, const R& right) noexcept {
        return static_cast<intptr_t>(std::tolower(left)) - static_cast<intptr_t>(std::tolower(right));
    }
};

using p = parser<std::string::const_iterator, int, int, case_insensitive_comparator>;

auto grammar = p::term("select");   // now also matches "SELECT", "Select", ...
```

## Debugging a grammar

Wrap any sub-expression in `p::debug(node, "tag")` to get a stable place to set a breakpoint (its `parse()` just forwards to the child) while stepping through a parse:

```cpp
auto value = p::debug(number | parenthesized_expression, "value");
```

You can also name any parse node/rule for introspection with `[]`: `expression["expr"]`, `rule.set_name("expr")`.

## Full example: a calculator

See the top of this file, or `examples/calculator.cpp` for the complete, runnable version (grammar + evaluator). It builds a full four-operator expression grammar with correct precedence and parentheses in well under 100 lines, using `left_associative` for `+ -` and `* /`.

## Examples and tests

- [`examples/calculator.cpp`](/examples/calculator.cpp) — arithmetic expression parser + evaluator, precedence via `left_associative`.
- [`examples/xml.cpp`](/examples/xml.cpp) — a small XML parser (tags, attributes, text nodes).
- [`examples/json.cpp`](/examples/json.cpp) — a full JSON reader, split into a tokenizer stage and a parser stage over the resulting tokens (see [Using the library as a lexer and a parser](#using-the-library-as-a-lexer-and-a-parser)), plus a renderer that reconstructs the original JSON from the match tree.
- [`tests/run_tests.cpp`](/tests/run_tests.cpp) — unit tests covering every combinator, terminal, and feature described above; also a good place to see minimal usage of each in isolation.

### Changes

- 1.0.0.12

	- rewrote the library into one file only, since it does not make sense as a group of files.
	- removed ASTs since they are redundant at the scope of the library; matches have children anyway.
	- removed support for left-recursive grammars due to a) being performance bottlenecks, due to many recursions, b) being difficult to follow mentally when debugging due to complexity, c) exceptions were used as non-local goto, which works but is not good code.
	- replaced left-recursive grammars with left-associative grammars; same effect, much easier to understand and debug, no exceptions as gotos required.

- 1.0.0.11

	- rewrote the library to allocate parse nodes on the heap and use type erasure, in order to speed up compilation; in previous versions it was ridiculously slow for non-trivial grammars and sometimes the compiler couldn't compile the code due to excessive use of templates.

- 1.0.0.10

	- changed the parse context API so it is also usable with stream iterators.
	- removed some features that were not really useful.
	- rules no longer need to be bound variables.
	- documentation.

- 1.0.0.9
	- Rewritten again from scratch in order to improve the quality.
	- changes:
		- renamed some functions in order to make more sense when the code is read.
		- improved support for catching errors and continuing.
		- more analytical documentation.
	- new features:
		- better debugging support.
		- grammar annotations.
		- extensible parse context.
		- infinite recursion exception.
		- loop breaks.
		- compile-time rule optimizations.

- 1.0.0.8
	- Rewritten again from scratch, in order to deal with error handling in a much better way,
  	  to make everything `noexcept` for increased performance,
      to add new capabilities.

- 1.0.0.7
    - Rewritten from scratch, to improve quality of the API.

- 1.0.0.6
	- Added function-based parsing.

- 1.0.0.5
	- Added custom match functions in order to allow the resolution of ambiguities while parsing.
  	- allowed terminal values to be of different type that the value of the source container, in order to allow the result of a parse (the ast nodes created by a parse) to be fed to another parse function.
  	- added terminal parsing via functions.
  	- added parsing via standalone functions.
  	- added multiple error handling.

- 1.0.0.4
    - Rewrote the library:
        - all parser grammar classes are now inside a single template class `class parser_engine<SourceT, MatchIdT>`, for the following reasons:
            - compiler performance (MSVC 32-bit regularly crashed with out of memory error from the many template instantiations of previous versions).
            - library code organization; writing a grammar usually requires including all the grammar constructs, so it is reduntant to have separate files for each grammar-related class.
            - user code organization; whole grammars need to be specialized on source type.
        - coding style is closer to the standard: all identifiers are lower case, words are separated by underscores, idiomatic c++ is used whenever possible.
    - Rewrote the documentation, due to more functionality to be added in the future.

- 1.0.0.3
 	- Reorganized the library in order to support compiler front ends into a separate namespace. The main library is now in `namespace parserlib::core`.
 	- Added `namespace parserlib::cfe` which now contains the compiler-front-end functionality.
 	- separated tokenization and parsing phases for compiler-front-ends.
 	- Added relevant documentation and unit tests.

- 1.0.0.2
 	- Rewrote the library from scratch in order to provide a better interface. Changes:
        - All the getter methods now start with 'get', in order to play better with Intellisense.
        - The `ParseContext` class is now configured over the Source type, with the default class being the class `SourceString`.
        - The class `SourceString` provides custom iterator which counts lines and columns, compatible with the `std::string` interface.
        - The functions `terminal`, `terminalSet`, `terminalRange` are changed to `term`, `oneOf`, `oneIn`.
        - Matches are now only hierarchical (as in `operator >=` of previous version).
        - The `'operator >=` has been replaced with `operator ->*`, which is much more distinct than the former; no more typing accidentally '>>' where `>=` was intended.
        - The default match id type is no longer a string; it is an int.
        - Simplified the left recursion parsing implementation.

- 1.0.0.1
	- Added support for compiler front-end construction.

- 1.0.0.0
	- Initial release.
