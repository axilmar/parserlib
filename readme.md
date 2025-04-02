# Parserlib

A `c++17` recursive-descent parser library with the following features:

- ENBF-like syntax for writing grammar expressions.
- character, string, boolean, set, range terminals.
- recursive and left-recursive grammars.
- custom parsing functions, callbacks and nodes.
- multiple errors per parse.
- case sensitive/insensitive parsing.
- optional line/column counting.
- parsing utf-8 strings.
- translation of source to AST nodes.
- loading a file directly into memory.
- parsing streams.
- lexing/parsing.

Version 1.0.0.8

## Quick Example

```cpp
#include "parserlib.hpp"
using namespace parserlib;

int main() {
    const auto grammar = terminal("parserlib is") >> " my favorite library" >> -terminal('!');
    std::string source = "parserlib is my favorite library!";
    parse_context<std::string> pc(source);
    bool success = grammar.parse(pc);
}
```

##### Table Of Contents

1. [EBNF-like syntax](doc/ebnf_like_syntax.md)
1. [Terminals](doc/terminals.md)
1. [Recursive Grammars](doc/recursion.md)
1. [The Parse Context](doc/parse_context.md)
1. [Custom Parse Functions/Callbacks/Parse Nodes](doc/custom_functions.md)
1. [Error Handling](doc/error_handling.md)
1. [Line/Column counting](doc/line_counting.md)
1. [UTF-8 strings](doc/utf8_strings.md)
1. [AST Nodes](doc/ast_nodes.md)
1. [Lexing And Parsing](doc/lexing_parsing.md)
1. [Parsing In-Memory Sources](doc/load_file.md)
1. [Parsing Sources From Streams](doc/parsing_streams.md)
1. [Versions](#versions)

### Versions

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

### TODO

TODO list in the current version:

 - review and improve the library documentation.
