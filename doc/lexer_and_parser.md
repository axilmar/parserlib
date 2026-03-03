# Parserlib

## Writing a lexer and a parser

The library can be used to write a lexer and a parser.

This is achieved by using a parse context for character parsing, and then a parse context for the matches of the lexer parse context.

* [The lexer](#the-lexer)
* [The parser](#the-parser)

### The lexer

Creating a lexer means to create a grammar around a parse context that parses characters.

It usually involves:

1) specifying an enumeration with lexer ids
2) specifying an enumeration lexer error ids
3) defining the tokens
4) creating a loop which also catches invalid characters

For example:

```cpp
//id for lexer items
enum class LEXER_ID {
	NUMBER,
    IDENTIFIER,
    STRING,
    LEFT_PARENTHESIS,
    RIGHT_PARENTHESIS,
    ASSIGNMENT,
    TERMINATOR
};

//lexer error id
enum class LEXER_ERROR_ID {
	INVALID_CHARACTERS
};

using lp = parser<std::string::const_iterator, LEXER_ID, LEXER_ERROR_ID>;

//whitespace
auto space = lp::range('\0', ' ');
auto comment = "(*" >> *(lp::any() - "*)") >> "*)";

//number
auto digit = lp::range('0', '9');
auto number = +digit >> -('.' >> digit);

//identifier
auto letter = lp::range('a', 'z') | lp::range('A', 'Z');
auto identifier = (letter >> *(letter | digit))->*LEXER_ID::IDENTIFIER;

//string
auto string = ('"' >> *(lp::any() - '"') >> '"')->*LEXER_ID::STRING;

//symbols
auto left_parenthesis = lp::terminal('(')->*LEXER_ID::LEFT_PARENTHESIS;
auto right_parenthesis = lp::terminal(')')->*LEXER_ID::RIGHT_PARENTHESIS;
auto assigment = lp::terminal('=')->*LEXER_ID::ASSIGNMENT;
auto terminator = lp::terminal(';')->*LEXER_ID::TERMINATOR;

//token
auto token = space | comment | identifier | string | left_parenthesis | right_parenthesis | assignment;

//lexer
auto lexer = *(token | lp::error(LEXER_ERROR_ID::INVALID_CHARACTERS, lp::skip_before(token)));
```

The lexer can be used like this:

```cpp
	std::string source = ...;
	lp::parse_context lexer_pc(source);
    const bool ok = lexer.parse(lexer_pc);
```

### The parser

Creating a parser from a lexer means to create a parser grammar around the lexer ids.

It usually involves:

1) specifying an enumeration with parser ids
2) specifying an enumeration parser error ids
3) defining the rules
4) creating the top level grammar for the parser

For example, a parser on the lexer defined above can be:

```cpp
enum class PARSER_ID {
	NAMED_VALUE,
    NUMERIC_LITERAL,
    STRING_LITERAL
};

enum class PARSER_ERROR_ID {
	...
};

using pp = parser<lp::match_container_type::const_iterator, PARSER_ID, PARSER_ERROR_ID>;

extern pp::rule expression;

auto named_value = pp::terminal(LEXER_ID::IDENTIFIER)->*PARSER_ID::NAMED_VALUE;

auto numeric_literal = pp::terminal(LEXER_ID::NUMBER)->*PARSER_ID::NUMERIC_LITERAL;

auto string_literal = pp::terminal(LEXER_ID::STRING)->*PARSER_ID::STRING_LITERAL;

auto value = named_value
           | numeric_literal
           | string_literal
           | LEXER_ID::LEFT_PARENTHESIS >> expression >> LEXER_ID::RIGHT_PARENTHESIS
           ;

pp::rule expression = *value;

auto declaration = pp::terminal(LEXER_ID::IDENTIFIER) >> LEXER_ID::ASSIGNMENT >> expression >> LEXER_ID::TERMINATOR;

auto parser = *declaration;
```

The parser can be used like this:

```cpp
auto parser_pc = lexer_pc.derive_parse_context<PARSER_ID, PARSER_ERROR_ID>();
parser.parse(parser_pc);
```

The member function `derive_parse_context` equals the following:

```cpp
pp::parse_context parser_pc(lexer_pc.get_matches());
```
