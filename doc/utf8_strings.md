# Parserlib

## UTF-8 Strings

The `parserlib` library provides the class `utf8_string<String = std::string>` which can be used as a source for parsing.

The parameter `String` represents the base string class, for example `std::string`.

Here is an example of parsing a utf-8 string:

```cpp
#include "parserlib.hpp"
using namespace std;

int main() {
    auto grammar =  terminal("hello world!");
	utf8_string source = "hello world!";
    parse_context<utf8_string> pc(source);
    grammar.parse(pc);
    
}
```

** NOTE: The class `utf8_string` is not a full replacement of `std::string`. It's main purpose is to provide a const_iterator which knows how to read unicode codepoints from a utf8 buffer. **
