# Parserlib

## Left recursion

`Parserlib` can handle left recursion grammars.

The way this is achieved is the following:

* a parse context keeps the last parse position used by a rule, and therefore can recognize when left recursion exists dynamically: when the new parse position for a rule is the same as the previous one, then there is a left recursion on that rule.
* upon recognizing left recursion, the parse context is rewinded to the last non-left recursive position for that rule. Then left recursion parsing starts, and it is executed in two phases:
	1. the `reject` phase, in which left recursive rules reject parsing in order to allow non-left recursive rules to parse.
	2. the `accept` phase, in which left recursive rules accept parsing without calling the parser, allowing the parts after the left recursive rules to be parsed.

The `accept` phase loops until no more parsing can be done.

The algorithm keeps the left-associativity of the grammar, by forcing matches to start from where the left recursion starts.

Let's see an example. Here is a left-recursive calculator grammar:

```cpp
const auto digit = range('0', '9');

const auto num = +digit >> -('.' >> +digit);

rule<> val = num
           | '(' >> add >> ')';

rule<> mul = mul >> '*' >> val
           | mul >> '/' >> val
           | val;

rule<> add = add >> '+' >> mul
           | add >> '-' >> mul
           | mul;
```

In the `reject` phase, the above grammar looks like this:

```cpp
const auto digit = range('0', '9');

const auto num = +digit >> -('.' >> +digit);

rule<> val = num
           | '(' >> add >> ')';

rule<> mul = REJECT >> '*' >> val
           | REJECT >> '/' >> val
           | val;

rule<> add = REJECT >> '+' >> mul
           | REJECT >> '-' >> mul
           | mul;
```

This allows the non-left recursive parts to be parsed first.

Then, in the `accept` phase, the grammar looks like this:

```cpp
const auto digit = range('0', '9');

const auto num = +digit >> -('.' >> +digit);

rule<> val = num
           | '(' >> add >> ')';

rule<> mul = *(ACCEPT >> '*' >> val
           | ACCEPT >> '/' >> val
           | REJECT);

rule<> add = *(ACCEPT >> '+' >> mul
           | ACCEPT >> '-' >> mul
           | REJECT);
```

The above algorithm allows for left-recursive grammars to be parsed, while maintaining left associativity.
