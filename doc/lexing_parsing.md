# Parserlib

## Lexing And Parsing

***NOTE**: the terms `lexer` and `parser` in the following texts refer to whole classes that contain the types and functions for tokenization of source and parsing of tokens, respectively.*

###### Table Of Contents

- [Lexing](#lexing)
	- [The Lexer Class](#the-lexer-class)
	- [The Lexer Parse Function](#the-lexer-parse-function)
	- [The Lexer Parse Result](#the-lexer-parse-result)
- [Parsing](#parsing)
	- [The Parser Class](#the-parser-class)
	- [The Parser Parse Function](#the-parser-parse-function)
	- [AST Factories](#ast-factories)
	- [The Parser Parse Result](#the-parser-parse-result)
- [Grammar Classes](#grammar-classes)
- [Best Practices For Writing Parsers](#best-practices-for-writing-parsers)
- [Sharing Error Ids Between Lexer And Parser](#sharing-error-ids-between-lexer-and-parser)

### Lexing

#### The lexer Class

In order to tokenize a source, `parserlib` provides the following class:

```cpp
template <class Source, class Grammar> class lexer;
```

The `lexer` class can be parameterized over the following types:

- `Source`: the source type to parse.
- `Grammar`: [the class that implements the grammar](#grammar-classes).

#### The Lexer Parse Function

The `lexer` class provides the following static functions that can be used for parsing:

```cpp
template <class Extension>
static result parse(Source& source, const Extension& extension) noexcept;
static result parse(Source& source) noexcept;
```

Parameters:

- `source`: the source to parse.
- `extension`: instance of extension to add to the parse context; optional; if not given, then an instance of `empty_parse_context_extension` class is used.

#### The Lexer Parse Result

The `parse` function returns the following structure:

```cpp
struct result {
    bool success;
    parsed_token_container_type parsed_tokens;
    error_container_type errors;
};
```

Members:

- `success`: true if parsing was successful:
	- if the grammar parsed successfully.
	- if the whole source was parsed.
	- if there were no errors.
- `parsed_tokens`: std::vector of parsed token instances.
- `errors`: std::vector of error instances.

### Parsing

#### The Parser Class

In order to tokenize a source, then parse the tokens and create an AST tree, `parserlib` provides the following class:

```cpp
template <class Source, class LexerGrammar, class ParserGrammar> class parser;
```

Parameters:

- `Source`: the source type to parse.
- `LexerGrammar`: [the class that implements the lexer grammar](#grammar-classes).
- `ParserGrammar`: [the class that implements the parser grammar](#grammar-classes).

#### The Parser Parse Function

The `parser` class provides the following static functions that can be used for parsing:

```cpp
template <class AstFactory, class Extension>
static result parse(Source& source, const AstFactory& ast_factory, const Extension& extension);

template <class ASTFactoryOrExtension>
static result parse(Source& source, const ASTFactoryOrExtension& ast_factory_or_extension) noexcept;

static result parse(Source& source) noexcept;
```

Parameters:

- `source`: the source to parse.
- `ast_factory`: the class that creates instances of AST nodes; it can be used to create subclasses of class [`ast_node`](./ast_nodes.md).
- `extension`: instance of extension to add to the parse context; optional; if not given, then an instance of `empty_parse_context_extension` class is used.
- `ast_factory_or_extension`: either an AST factory class, or an extension class, or a class that is both an AST factory and a parse context extension.

#### AST Factories

If no customization of creation of the AST node instances is required, then the `parser` class provides the class `default_ast_factory` which simply creates a shared instance of class `ast_node` from the given parameters.

If an AST factory needs to create custom subclasses of class `ast_node`, then it must provide the following interface (example):

```cpp
enum class MY_NODE_ID {
	A,
    B,
    OTHER
};

/* custom ast nodes */

template <class Iterator>
class ast_node_a : public ast_node<MY_NODE_ID, Iterator> {
public:
	ast_node_a(
        const Iterator& begin, 
        const Iterator& end, 
        std::vector<std::shared_ptr<ast_node<MY_NODE_ID, Iterator>>>&& children)
    : ast_node<MY_NODE_ID, Iterator>(MY_NODE_ID::A, begin, end, std::move(children)))
    {    
    }
};

template <class Iterator>
class ast_node_b : public ast_node<MY_NODE_ID, Iterator> {
public:
	ast_node_b(
        const Iterator& begin, 
        const Iterator& end, 
        std::vector<std::shared_ptr<ast_node<MY_NODE_ID, Iterator>>>&& children)
    : ast_node<MY_NODE_ID, Iterator>(MY_NODE_ID::B, begin, end, std::move(children)))
    {    
    }
};

//custom ast factory
struct my_ast_factory : ast_factory_base {
    template <class Iterator>
	std::shared_ptr<ast_node<MY_NODE_ID, Iterator>> 
    operator ()(
    	const MY_NODE_ID& node_id, 
        const Iterator& begin, 
        const Iterator& end, 
        std::vector<std::shared_ptr<ast_node<MY_NODE_ID, Iterator>>>&& children) const noexcept
    {
    	switch (node_id) {
        	case MY_NODE_ID::A:
    			return std::make_shared<ast_node_a<MY_NODE_ID, Iterator>>(begin, end, std::move(children));
        	case MY_NODE_ID::B:
    			return std::make_shared<ast_node_b<MY_NODE_ID, Iterator>>(begin, end, std::move(children));
            default:
                break;
        }
        return std::make_shared<ast_node<MY_NODE_ID, Iterator>>(node_id, begin, end, std::move(children));
    }
};
```

#### The Parser Parse Result

The `parse` function returns the following structure:

```cpp
struct result {
    bool success;
    ast_node_container_type ast_nodes;
    error_container_type errors;
	typename lexer_type::result lexer;
};
```

Members:

- `success`: true if parsing was successful:
	- if the grammar parsed successfully.
	- if the whole source was parsed.
	- if there were no errors.
- `ast_nodes`: std::vector of shared ptr instances to ast node instances.
- `errors`: std::vector of error instances, including both lexing and parsing errors, if they share the same error id type.
- `lexer`: the [lexer parse result](#the-lexer-parse-result).

### Grammar Classes

In order to use the standalone lexer or the parser, one has to write a 'grammar' class. 

A 'grammar' class is a class that shall contain the following:

- enum type `match_id_type`: enumeration of the match id.
- enum type `error_id_type`: enumeration of the error id; optional.
- functor type `comparator_type`: type of comparator between input tokens and terminals; optional. If not defined, then the functor `case_sensitive_comparator` is used, which simply computes the difference between tokens and terminals, using the binary `operator -`.
- function `template <class ParseContext> parse_result parse(ParseContext& pc) const noexcept`: function to use for parsing.

Here is an example of a lexer class that parses a list of comma-separated integers:

```cpp
class integer_sequence_lexer_grammar {
public:
	enum class match_id_type {
        WHITESPACE,
        COMMA,
    	INTEGER
    };
    
    //optional
    enum class error_id_type {
    	INVALID_TOKEN
    };
    
    template <class ParseContext>
    parse_result parse(ParseContext& pc) const noexcept {
    	const auto digit = range('0', '9');

        //terminals
        const auto whitespace = *terminal(' ')->*match_id_type::WHITESPACE;
        const auto comma = terminal(',')->*match_id_type::COMMA;
        const auto integer = +digit->*match_id_type::INTEGER;

        //token
        const auto token
        	= whitespace
            | comma
            | integer
            | error(error_id_type::INVALID_TOKEN, skip_until(set(" ,0123456789")));
        
        //grammar
        const auto grammar = *token;

        //parse
        return grammar.parse(pc);
    }
};
```

### Best Practices For Writing Parsers

#### Sharing Error Ids Between Lexer And Parser

A good practice is to share error ids between a lexer and a parser. In this way, at the end of parsing, the error container will contain both lexer and parser errors, allowing a single error report function to be used.

For example:

```cpp
struct my_language_grammar {
    enum class MY_LANGUAGE_ERROR_ID {
        ...
    };
};

struct my_language_lexer_grammar : my_language_grammar {
};

struct my_language_parser_grammar : my_language_grammar {
};
```
