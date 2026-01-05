# Parserlib

## Abstract Syntax Trees

The library provides facilities for converting the matches of a parse context to an Abstract Syntax Tree.

An AST tree is composed of AST nodes, where each AST node has an id, a begin and end iterator, and a container of children.

The difference between matches and AST nodes is that AST nodes can be polymorphic; they usually contain methods for a compiler front-end, or even evaluation member functions for interpreters.

### The make_ast_node function

In order to convert a match and its children to an AST, the following function can be used:

```cpp
template <class Id, class Iterator, class ASTFactory>
std::shared_ptr<ast_node<Id, Iterator>> make_ast_node(const match<Id, Iterator>& m[, ASTFactory& factory]);
```

It creates a tree of AST nodes recursively, with the help of an AST node factory.

The AST node factory is optional; if ommitted, a default AST node factory is used.

### The AST node factory class

An AST node factory class must provide the following interface:

```cpp
class my_ast_factory {
public:
    template <class Id, class Iterator>
    std::shared_ptr<ast_node<Id, Iterator>> operator ()(const match<Id, Iterator>& m);
};
```

The library provides an implementation of an AST node factory which creates a basic AST node without any polymorphism:

```cpp
class default_ast_factory {
public:
    template <class Id, class Iterator>
    std::shared_ptr<ast_node<Id, Iterator>> operator ()(const match<Id, Iterator>& m) {
        return std::make_shared<ast_node<Id, Iterator>>(m.get_id(), m.begin(), m.end());
    }
};
```

