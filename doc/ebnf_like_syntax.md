# Parserlib

## EBNF-like syntax

[Operator Table](operator-table)

[Loop Zero Or More Times](loop-zero-or-more-times)

[Loop One Or More Times](loop-one-or-more-times)

[Optional](optional)

[Logical And](logical-and)

[Logical Not](logical-not)

[Sequence](sequence)

[Choice](choice)

[Exclusion](exclusion)

[Callback][callback]

### Operator Table

The library offers the following operators for writing a grammar using an EBNF-like syntax:

|  Operator  |     Meaning                      | Example                            |      Notes                      |
|------------|----------------------------------|------------------------------------|---------------------------------|
| * (unary)  | loop zero or more times          | `*terminal('a')`                   |                                 |
| + (unary)  | loop one or more times           | `+terminal('a')`                   |                                 |
| - (unary)  | optional                         | `-terminal('a')`                   |                                 |
| &          | logical and                      | `&terminal('a')`                   | does not consume input          |
| !          | logical not                      | `!terminal('a')`                   | does not consume input          |
| >>         | sequence                         | `terminal('a') >> 'b'`             |                                 |
| \|         | choice                           | `terminal('a') | 'b'`              |                                 |
| - (binary) | exclusion                        | `terminal('a') - 'b'`              |                                 |
| []         | callback                         | `terminal('a')[my_callback]`       |                                 |

### Loop zero or more times

The unary `operator *` can be used with grammar expressions to create a parsing loop that parses an expression zero or more times.

The loop stops when the expression cannot be parsed or when the input ends.

The `operator *` always returns true, since the loop is optional.

Examples:

```cpp
#include "parserlib.h"
using namespace parserlib;

//parses "", a", "aa", "aaa" etc.
static const auto list_of_letter_a = *terminal('a');
```

### Loop one or more times

The unary `operator +` can be used with grammar expressions to create a parsing loop that parses an expression one or more times.

The loop stops when the expression cannot be parsed or when the input ends.

The `operator +` returns false if the grammar fails to parse at least once, otherwise it returns false.

Examples:

```cpp
#include "parserlib.h"
using namespace parserlib;

//parses "a", "aa", "aaa" etc.
static const auto list_of_letter_a = +terminal('a');
```

### Optional

The unary `operator -` can be used with grammar expressions to create an optional expression.

It always returns true.

Examples:

```cpp
#include "parserlib.h"
using namespace parserlib;

//parses "" and "a".
static const auto optional_letter_a = -terminal('a');
```

### Logical And

The unary `operator &` can be used with grammar expressions to create a testing predicate.

It does not consume any input.

It returns what the expression returns.

Examples:

```cpp
#include "parserlib.h"
using namespace parserlib;

//It will test the input for 'a'.
static const auto test_letter_a = &terminal('a');
```

### Logical Not

The unary `operator !` can be used with grammar expressions to create a testing predicate which is the negation of the result of another grammar expression.

It does not consume any input.

It returns the opposite of what the expression returns, i.e. of the expression returns true, then the operator returns false, and vice versa.

Examples:

```cpp
#include "parserlib.h"
using namespace parserlib;

//It will test the input does not start with 'a'.
static const auto test_not_letter_a = !terminal('a');
```

### Sequence

The `operator >>` can be used to declare a sequence of grammar expressions.

The operator returns true if all the members of the sequence return true.

Otherwise, it returns false and the parse position is rewind to the position before this operator is called.

Members of the sequence that are terminal values are automatically converted to terminal expressions.

The members are called in the order they are declared.

Examples:

```cpp
#include "parserlib.h"
using namespace parserlib;

//parses "abc"
static const auto abc_sequence = terminal('a') >> 'b' >> 'c';
```

### Choice

The `operator |` can be used to declare a choice of grammar expressions.

The operator returns true if at least one member returns true.

Otherwise, it returns false and the parse position is rewind to the position before this operator is called.

Members of the choice that are terminal values are automatically converted to terminal expressions.

The members are called in the order they are declared.

Examples:

```cpp
#include "parserlib.h"
using namespace parserlib;

//parses "a", or "b", or "c".
static const auto a_or_b_or_c = terminal('a') | 'b' | 'c';
```

### Exclusion

The `operator -` can be used to exclude some grammar from another grammar.

The operator turns an expression of type `a - b` to the expression `!b >> a`.

Examples:

```cpp
#include "parserlib.h"
using namespace parserlib;

//parses all letters except 'a'
static const auto parse_letter_except_a = letter - 'a';
```

### Callback

The `operator []` can be used to add a callback to an expression which will be called as soon as the expression finishes parsing.

The callback must have the following signature:

```cpp
	template <class ParseContext>
    parse_result my_callback(
    	ParseContext& context, 
        const parse_result& expression_result, 
        typename ParseContext::iterator_type& start_position, 
        typename ParseContext::iterator_type& end_position)
    {
       ...
    }
```

Parameters:

- `context`: the parse context used for parsing.
- `expression_result`: the result of the expression that parsed the input.
- `start_position`: position in the source from which the expression start parsing.
- `end_position`: last position in the source that the expression parsed.

The callback function must return a `parse_result` instance.

Examples:

```cpp
#include "parserlib.h"
using namespace parserlib;

const auto letter_a_callback = [](auto& context, const parse_result& result, const auto& start, const auto& end){
	std::cout << "parsed " << source(start, end) << std::endl;
    return result;
}

const auto parse_letter_a = terminal('a')[letter_a_callback];
```
Note: the function `source` in the above example is a standalone parserlib function that returns part of an input as a string view (for integral elements) or a vector (for non-integral elements).

