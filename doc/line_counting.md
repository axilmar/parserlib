# Parserlib

## Line Counting

`Parserlib` contains the class `line_counting_string<String = std::string>` which can be used to add line counting to `String::const_iterator`.

The parameter `String` represents the base string class, for example `std::string` or `utf8_string`.

For example:

```cpp
#include "parserlib.hpp"
using namespace std;

int main() {
    auto grammar =  terminal("hello world!");
	line_counting_string<> source = "hello world!";
    parse_context<line_counting_string<>> pc(source);
    grammar.parse(pc);
    
}
```

The `line_counting_string` class allows viewing the `line` and `column` of each parse position, which is useful when reporting errors to the user.

