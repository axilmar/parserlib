# Parserlib

## Parsing Streams

`Parserlib` contains the following class:

```cpp
template <
	class Stream, 
    class Buffer = std::basic_string<typename Stream::char_type>, 
    std::size_t ReadAheadCount = 4096>
class stream_string;
```

Which allows a stream to be combined with a string, and the stream to be read when more characters are needed, lazilly.

The parameters of the class are:

- `Stream`: type of STL-like stream to use as input.
- `Buffer`: type of STL-like container to place data read from the stream.
- `ReadAheadCount`: number of elements to read from the string, when new data are required.

Here is an example:

```cpp
const int DIGIT = 1;
const int NEWLINE = 2;

const auto digit = range('0', '9')->*DIGIT;
const auto line_end = terminal("\r\n")->*NEWLINE;
const auto grammar = *(digit | line_end);

using StreamString = stream_string<std::stringstream>;
using ParseContext = parse_context<StreamString>;

std::string inputString = "123\r\n456\r\n789";
std::stringstream inputStream;
inputStream << inputString;
StreamString source(inputStream);

ParseContext context(source);

assert(grammar.parse(context));
assert(context.is_end_parse_position());

assert(context.matches().size() == 11);
```
