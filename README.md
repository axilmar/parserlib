# Parserlib

A c++17 Parsing Expression Grammar (PEG) recursive-descent library.

**Current Version**

1.0.0.10

**Features**

- PEG parser with EBNF-like syntax.
- recursive-descent parsing.
- parses left-recursive grammars.
- can be used as a lexer and parser.
- supports case insensitive parsing.
- supports line and column.
- can be used on STL containers and streams.
- supports the creation of ASTs.

**Quick Example: Calculator**

```cpp
extern rule add;

//digit
auto digit = range('0', '9');

//number
auto number = +digit >> -('.' >> +digit);

//value
auto value = '(' >> add >> ')'
           | number;

//multiplication/division
rule mul = mul >> '*' >> value
         | mul >> '/' >> value
         | value;

//addition/subtraction
rule add = add >> '+' >> mul
         | add >> '-' >> mul
         | mul;
```

## Table Of Contents

* [Writing a parser](doc/writing_a_parser.md)
* TODO documentation
* [Changes](#changes)

### Changes

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