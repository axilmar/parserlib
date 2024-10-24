# Parserlib

A `c++17` recursive-descent parser library that can parse left-recursive grammars.

Version 1.0.0.4.

## Table Of Contents

[Introduction](#introduction)

[Writing Parsers](./doc/writing_parsers.md)

[Versions](#versions)

## Introduction

Parserlib is a `c++17` library that allows the creation of recursive-descent parsers, based on `EBNF`-like notation using the available `c++` operators.

The library allows the declaration of left-recursive grammars.

Example:

```cpp
#include "parserlib.hpp"

template <typename SourceT = std::string> class calculator_grammar {
public:
    enum class ast_id {
        number,
        add,
        sub,
        mul,
        div
    };

    using pe = parserlib::parser_engine<SourceT, ast_id>;

    calculator_grammar() {
        auto digit = pe::range('0', '9');

        auto number = (-pe::one_of("+-") >> +digit >> -('.' >> +digit))->*ast_id::number;

        auto val = '(' >> m_add >> ')'
                 | number;

        m_mul = (m_mul >> '*' >> val) ->* ast_id::mul
              | (m_mul >> '/' >> val) ->* ast_id::div
              | val;

        m_add = (m_add >> '+' >> m_mul) ->* ast_id::add
              | (m_add >> '-' >> m_mul) ->* ast_id::mul
              | m_mul;
    }

    double evaluate(const SourceT& input) {
        SourceT source = input;
        auto [success, ast, it] = pe::parse(source, m_add);
        if (success && ast.size() == 1) {
            return evaluate_ast(ast[0]);
        }
        throw std::invalid_argument("Parse error: " + std::string(it, source.cend()));
    }

private:
    typename pe::rule m_add, m_mul;

    static double evaluate_ast(const typename pe::ast_node_ptr_type& node) {
        switch (node->get_id()) {
            case ast_id::number: {
                std::stringstream stream;
                stream << node->get_source();
                double r;
                stream >> r;
                return r;
            }

            case ast_id::add:
                return evaluate_ast(node->get_children()[0]) + evaluate_ast(node->get_children()[1]);

            case ast_id::sub:
                return evaluate_ast(node->get_children()[0]) - evaluate_ast(node->get_children()[1]);

            case ast_id::mul:
                return evaluate_ast(node->get_children()[0]) * evaluate_ast(node->get_children()[1]);

            case ast_id::div:
                return evaluate_ast(node->get_children()[0]) / evaluate_ast(node->get_children()[1]);
        }

        throw std::logic_error("invalid ast id");
    }
};

int main() {
	calculator_grammar<> calculator;
    std::cout << calculator.evaluate("1+(5*6)/2") ;
}
```

The above prints `16` at the console.

## Versions

  - 1.0.0.4
  
    - Rewrote the library:
        - all parser grammar classes are now inside a single template class `class parser_engine<SourceT, MatchIdT>`, for the following reasons:
            - compiler performance (MSVC 32-bit regularly crashed with out of memory error from the many template instantiations of previous versions).
            - library code organization; writing a grammar usually requires including all the grammar rules, so it is reduntant to have separate files for each grammar-related class.
            - user code organization; whole grammars need to be specialized on source type.
        - coding style is closer to the standard: all identifiers are lower case, words are separated by underscores, idiomatic c++ is used whenever possible.
 
 
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

