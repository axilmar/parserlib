# Parserlib

### Loading Files Into Memory

`Parserlib` contains the function `load_file`:

```cpp
template <class Path, class Dst> bool load_file(const Path& path, Dst& dst);
template <class Char, class Dst> bool load_file(const Char* path, Dst& dst);
```

It allows loading a file directly into a string instance (std::string, utf8_string, etc), which can then be used as input for parsing.

For example:

```cpp
int main() {
    //std::string str;
	utf8_string str;
	load_file("my_file.txt", str);
}
```
