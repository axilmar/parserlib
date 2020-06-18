# How parsing is implemented.

Parserlib implements parsing by

1. creating trees of parser objects via operators >>, |, * etc.
2. invoking the root parser object with a parse context object.

A parse context object (class `parse_context`) contains the state of parsing, which includes:

1. the current input position.
2. the table of matches.
3. left recursion state.

If a parser object succeeds in parsing, it returns `parse_result::accepted`, while if it fails, it returns `parse_result::rejected`.

The generic parser interface is the following:

	template <class ParseContext> parse_result parse(ParseContext& pc) const;

All parser classes implement the above interface.

Some parser classes take decisions based on the state of parsing.

For example, the sequence parser has two children parsers A and B; it first invokes parser A, and if that succeeds, then it invokes parser B.

The choice parser also has two child parsers A and B; it first invokes parser A, and if that fails, then it invokes parser B.

In similar fashion, other parsers like the loop keep parsing their only child until failure or end of input, etc.

## recursive parsing.

The `class rule` is special because it contains a polymorphic interface to parsing. This is necessary because rules can be invoked recursively. The following does not compile:

	auto rule1 = '{' >> rule1 >> '}';

Because `rule1` cannot be used before it is defined. However, this works:

	rule<> rule1 = '{' >> rule1 >> '}';

The above works because rules are kept by reference in a grammar tree, and thus they do not need to be fully defined when used.

This allows for recursive grammars.


 

