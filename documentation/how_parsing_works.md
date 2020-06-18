# How parsing works

What is parsing? it is, in essence, an attempt to match input with a set of rules...it is a giant loop of alternating and nested if-then-else blocks which test the input, and if there a match is found, and event is raised...until an error happens or the input is exhausted.

Following this logic, parserlib is nothing more than a way to organize these if-then-else blocks into such a way that the declaration of objects creates the desired order of if-then-else blocks.

A parserlib expression like the following:

	'{' >> something >> '}'

means 

1. parse '{' then
2. parse 'something' then
3. parse '}'

A parserlib expression like the following:

	number | string | boolean | null

means

1. parse number; if that fails
2. parse string; if that fails
3. parse boolean; if that fails
4. parse null.

And that is how parsing works in parserlib.

Parserlib actually implements a PEG parser, i.e. a Parsing Expression Grammar parser, which means it tries to invoke the parsing functions as they are read, from left to right and top to bottom order. This has certain advantages against table-driven parsers, which attempt to parse based on instructions from a table: PEG parsers are more generic, they don't have to have context-free grammars, they don't result in reduce-reduce or shift-reduce conflicts; also, it has some disadvantages against table driven parsers, namely that it can be slower to parse with PEG parsing, because a lot of times the same section of the input may be parsed over and over.

One of parserlib's advantages is that it can handle left recursion, as mentioned in [this document](file://how_left_recursion_is_handled.md "how left recursion is handled"). 
