# Parserlib

## Abstract Syntax Tree (AST) Nodes

The library `parserlib` provides the class [`ast_node`](../include/parserlib/ast.hpp#ast_node) to allow the conversion of matches to an AST tree.

###### Table Of Contents

1. [Memory Management](#memory-management)
1. [Class Signature](#class-signature)
1. [Class Constructor](#class-constructor)
1. [Class Destructor](#class-destructor)
1. [Class Methods](#class-methods)

### Memory Management

AST nodes are managed via shared pointers, in order to allow the sharing of nodes with other parts of a program.

### Class Signature

The class `ast_node` has the following signature:

```cpp
    template <class NodeId, class Iterator>
    class ast_node : public std::enable_shared_from_this<ast_node<NodeId, Iterator>>;
```

The template parameters of the class are the following:

- `NodeId`: type of the node id. Usually, it is the match id given to the parse context used for parsing.
- `Iterator`: the type of the source iterator, used for keeping the position in the source to which the ast node corresponds to.

Example:

```cpp
using source_type = std::string;
enum AST_NODE_ID {
    NODE_A,
    NODE_B
};
using my_ast_node = ast_node<AST_NODE_ID, typename source_type::const_iterator>;
```

### Class Constructor

The class `ast_node` has the following constructor:

```cpp
ast_node(
	const node_id_type& node_id, 
    const iterator_type& begin, 
    const iterator_type& end, 
    ast_node_container_type<node_id_type, iterator_type>&& children) 
    noexcept
```

Parameters of the constructor:

- `node_id`: id of the node.
- `begin`: start position in the source.
- `end`: end position in the source.
- `children`: vector of shared ptrs to children AST nodes.

### Class Destructor

The destructor of the class `ast_node` is marked as virtual, in order to allow subclasses to be part of the tree.

Subclassing the class `ast_node` is not obligatory, it often can be used as is, but in some cases extra information might need to be stored in an AST node.

### Class Methods

The class `ast_node` contains the following methods:

```cpp
const node_id_type& id() const noexcept;
const iterator_type& begin() const noexcept;
const iterator_type& end() const noexcept;
ast_node_ptr_type<node_id_type, iterator_type> parent() const noexcept;
const ast_node_container_type<node_id_type, iterator_type>& children() const noexcept;
auto source() const noexcept;
```

The methods of the class are the following:

- `id()`: returns the id of the node, as specified in the constructor.
- `begin()`: returns the beginning of the source that the node corresponds to, as specified in the constructor.
- `end()`: returns the end of the source that the node corresponds to, as specified in the constructor.
- `parent()`: returns the parent node, or a null pointer if there is no parent node.
- `children()`: returns the children of the node, as specified in the constructor.
- `source()`: returns the portion of the source that corresponds to the given ast node.
