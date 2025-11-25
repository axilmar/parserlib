## Parserlib

## Debugging a parser

* [Debugging via 'debug'](#debugging-via-'debug')
* [Debugging via annotations](#debugging-via-annotations)
* [Debugging by inspecting the field 'm_text'](#debugging-by-inspecting-the-field-'m_text')

#### Debugging via 'debug'

The `debug` function wraps a parse node into an instance of `debug_parse_node`, which can be used to set a breakpoint into.

For example:

```cpp
const auto c = ...;
const auto expression = a >> b >> debug(c) >> d >> e;
```

By wrapping `c` into a `debug` parser, and setting up a breakpoint here ([file debug_parse_node.hpp](include/parserlib/debug_parse_node.hpp)):

```cpp
template <class ParseContext>
bool parse(ParseContext& pc) const {
    //put breakpoint here
    const bool result = m_child.parse(pc);
    return result;
}

```

one can debug a particular part of a grammar (`c` in the above example).

#### Debugging via annotations

The `operator []` can be used on parse nodes to add an annotation to them.

An annotation is any value the user wants; for debugging, it usually is the name/role of an expression.

By default, annotations are not processed; an extension is required that provides the function `parse_annotation(parse_context&, parse_node, annotation)`.

`Parserlib` already provides two such implementations:

1. `class default_debug_annotations_extension`: an empty implementation of the debug annotations extension.
2. `class debug_annotations_extension<OutputStream = std::ostream>`: an implementation of the debug annotations extension which prints the annotations as a tree, in an output stream of choice.

The type alias `default_debug_annotations_extension_type` defaults to `class default_debug_annotations_extension`.

Here is an example:

```cpp
int main() {
	const auto a = terminal('a')["A"];
	const auto c = terminal('c')["C"];
	const auto d = terminal('d')["D"];
	const auto b = (c | d)["B"];
	const auto grammar = (*(a | b))["Grammar"];
    
    std::string source = "aca";
    
    parse_context<
    	std::string, 
        int, 
        int, 
        default_text_position_type, 
        default_symbol_comparator_type, 
        debug_annotations_extension<>>
        pc(source);
    
    grammar.parse(pc);
}
```

The above code produces the following output:

```
Parsing Grammar at index 0
    Parsing A at index 0
    Successfully parsed A
    Parsing A at index 1
    Failed to parse A
    Parsing B at index 1
        Parsing C at index 1
        Successfully parsed C
    Successfully parsed B
    Parsing A at index 2
    Successfully parsed A
Successfully parsed Grammar
```

The default output stream for annotations is `std::cout` (or `std::wcout` if the type of output stream is `std::wostream`), but it can be customized:

1. by setting the output stream with the function `pc.set_output_stream(&stream)`.
2. by setting the template output stream type of the class `debug_annotations_extension`.

Examples:

```cpp
//set the output stream to be a file
std::ofstream file_stream(...);
pc.set_output_stream(&file_stream);

//declare another parse context which uses wchar_t for annotations:
parse_context<
	std::string, 
    int, 
    int, 
    default_text_position_type, 
    default_symbol_comparator_type, 
    debug_annotations_extension<std::wostream> 
    pc1(source);
```

#### Debugging by inspecting the field 'm_text'

Each parse node has a field `m_text` which is set to the textual description of the parse node.

This is valid only in debug mode.

For example, some of the JSON parser `m_text` member values are:

```
//text for 'object' expression
((LEFT_CURLY_BRACKET >> -(object_member >> *(COMMA >> object_member)) >> RIGHT_CURLY_BRACKET) ->* OBJECT)

//text for 'value' expression
(string | number | object | array | true | false | null)
```

