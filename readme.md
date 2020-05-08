# Parserlib #

Parserlib is a PEG parser library with the following features:

- it is written in C++17.
- it utilizes C++ operators to make the grammar readable like EBNF.
- it is very versatile in ways to declare connections of AST to the grammar.
- it supports left recursion.
- it is very versatile in supporting any type of symbol or input stream.

## Version ##

The current version is 0.0.4.

## Installation ##

Copy the contents of the include and source folders in your project.

## Folders ##

- include: include files; copy them verbatim to your project.
- source: source files; copy them verbatim to your project.
- examples: some examples and tests.

## Example ##

Here is an example of a calculator grammar, along with AST node declarations:

	extern Rule<> expr;
	
	Rule<> num = +range('0', '9')        == ast<Number>;
	
	Rule<> val = '(' >> expr >> ')'
	           | num;
	
	Rule<> mul = mul >> '*' >> val       == ast<Mul>
	           | mul >> '/' >> val       == ast<Div>
	           | val;
	
	Rule<> add = add >> '+' >> mul       == ast<Add>
	           | add >> '-' >> mul       == ast<Sub>
	           | mul;
	
	Rule<> expr = add;

You can find the complete calculator example in file [examples/calculator2.cpp](file://examples/calculator2.cpp "examples/calculator2.cpp").

## To do... ## 

2. support for multiple error reporting.
2. unit tests.
2. port examples to new version.
3. write new and better examples (well, maybe :-)).

## Versions ##

- 0.0.4: introduced new way of declaring AST node creations.
- 0.0.3: xml example.
- 0.0.2: rewritten in C++17, solved bugs.
- 0.0.1: the old version.

