# How to write grammars with parserlib.

Writing grammars with parserlib is very easy and straightfoward: you use the provided operators and functions to create a piece of code that resembles EBNF syntax.

## operators

The following operators can be used to create parsers:

- operator >> : creates a sequence; all elements of the sequence must parse correctly in order for the sequence to parse correctly.
- operator | creates a choice; at least one element of a choice must parse correctly in order for the choice to parse correctly.
- operator - : makes part of the grammar optional.
- operator * : creates a loop.
- operator + : creates a loop that must parse at least once to be succcessful.
- operator & : creates a logical and parser: the parser must parse successfully but it does not consume any input.
- operator ! : creates a logical and parser: the parser must parse successfully but it does not consume any input; it negates the result of the enclosed expression.
- operator ~ : forces an expression to report an error if it does not parse successfully, ignoring other branches of the enclosing choice.
- operator == : adds a match if part of a grammar is parsed successfully.

## functions

The following functions can be used to create parsers:


- terminal: creates a terminal out of a string or character.
- range: creates a range of characters.
- one_of: creates a set of characters.

## example

In order to parse an alphanumeric string, the following grammar shall be written:

	const auto digit = range('0', '9');
	const auto letter = range('a', 'z') | range('A', 'Z');
	const auto alphanumeric = +(digit | letter);

The above can be read like this: 

	an alphanumeric is a sequence of at least one digit or letter.

## recursive rules

Most of the time there is a need in grammars to recursively refer to rules. For this, the special class `rule` can be used for referring to recursive parts of a grammar.

For example, if we wanted to parse json, our grammar could be:

    extern rule<> json_object;
	auto json_value = json_string | json_number | json_object | json_array | json_boolean | json_null;
	auto json_data = json_name >> ':' >> json_value;
	rule<> json_object = '{' >> *json_data >> '}';

The library automatically takes care of adding references to rule objects where needed.

## matches

In order to identify a piece of input as matched, the user has to add tags to the grammar, using the `operator ==`.

For example, if I wanted to identify an alphanumeric as a specific matched input, I could write the grammar like this:

	const auto alphanumeric = +(digit | letter) == "alphanumeric";

Later, when the above parser parses successfully, a match with the tag 'alphanumeric' will be added to the match table of the parse context.
