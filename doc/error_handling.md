# Parserlib

## Error Handling

###### Table Of Contents

1. [Error Functions](#error-functions)
2. [Skip Policies](#skip-olicies)
    3. [Skip Nothing Policy](#skip-nothing-policy)
    4. [Skip Until Policy](#skip-until-policy)
    5. [Skip Until After Policy](#skip-until-after-policy)
    6. [Skip While Policy](#skip-while-policy)
    7. [Skip Count Policy](#skip-count-policy)
8. [Error Matches](#error-matches)

### Error Functions

`Parserlib` does not handle errors automatically, since for a recursive descent parser, it is impossible to tell which grammar expression failed due to error or which grammar expression failed because an alternative succeeded.

In order to aid error handling, `parserlib` contains the following functions which place an error in the parse context and optionally allow input to be skipped so as that parsing continues from a good position:

```cpp
template <class ErrorId> auto error(const ErrorId& error_id) noexcept;
template <class ErrorId, class SkipPolicy> auto error(const ErrorId& error_id, const SkipPolicy& skip_policy) noexcept;
```
Parameters:

- `error_id`: id of error to put in the parse context.
- `skip_policy`: optional skip policy to use; allows the parse position to be moved to a position that allows parsing to continue.

Error functions can be placed as alternatives to grammar expressions. For example:

```cpp
auto expression
	= expression1
    | expression2
    | error(ERROR1);
```

Error parse nodes always return true from parsing functions (`parse`, `parse_left_recursion_start`, `parse_left_recursion_continuation`), in order to allow parsing to continue.

### Skip Policies

A skip policy is a special parse node, also inheriting from `class skip_policy_base`, which allows the parse context iterator to be moved to a token that is a good position for continuing parsing.

#### Skip Nothing Policy

The helper function `skip_nothing()` returns a skip policy parse node which does nothing.

It is provided as the default value for an empty skip policy.

#### Skip Until Policy

The helper function `skip_until(parser)` creates a skip policy pase node which skips input until the given parser parses successfully.

For example:

```cpp
error(ERROR1, skip_until(';'));
```

The declaration above will put the error ERROR1 to the parse context and will move the parse position before the token ';', if there is one, otherwise it will move the iterator to the end of the input.

#### Skip Until After Policy

The helper function `skip_until_after(parser)` creates a skip policy pase node which skips input until the given parser parses successfully; it places then the parse context iterator to the position after the given parser parsed.

For example:

```cpp
error(ERROR1, skip_until_after(';'));
```

The declaration above will put the error ERROR1 to the parse context and will move the parse position after the token ';', if there is one, otherwise it will move the iterator to the end of the input.

#### Skip While Policy

The helper function `skip_until(parser)` creates a skip policy parse node which skips the input while the given parser parses successfully.

For example:

```cpp
error(ERROR1, skip_while(is_alphanumeric));
```

The declaration above will put the error ERROR1 to the parse context and will move the parse position to the first position that is not an alphanumeric character.

#### Skip Count Policy

The helper function `skip_count(<number>)` creates a skip policy parse node that skips a specific amount of tokens, until the end of input.

The helper function `skip_current()` skips one token.

### Error Matches
Sometimes, when there is an error, a missing construct should be replaced with an error construct, in order not to mess the order of matches.

The following functions allow the addition of a match into a parse context when a grammar expression fails to parse:

```cpp
template <class MatchId>
auto error_match(const MatchId& match_id) noexcept;
template <class MatchId, class SkipPolicy> auto error_match(const MatchId& match_id, const SkipPolicy& skip_policy) noexcept;
```

Example:

```cpp
auto expression
	= expression1
    | expression2
    | error_match(MATCH_ERROR1);
```
