## Code style {#code-style}

Please follow these guidelines for new code. We are migrating old code over the time.

### Code formatting

[ClangFormat](https://clang.llvm.org/docs/ClangFormat.html) allows to define code style rules and format source files automatically.
A `.clang-format` file is provided with the repository. Please use this file to format all sources.

### Naming conventions

For all new code, the following rules apply.

* type names and template parameter: `CamelCase`
* variable and function names: `snake_case`
* compiler macros and defines: `ALL_UPPERCASE`
* enum values: `UPPERCASE`
* (private) class members: start with `m_` respectively `mp_` for pointers and `mr_ ` for references 
* type traits: `snake_case` and end with `_type`
* namespace: `snake_case`

### Use of classes, structs and functions

We follow a Rust-style approach where we define data structures and attach basic operations that as methods to it. All functionality that can be considered optional is realized via free functions.

### Directory structure and namespaces 

* Libraries
    * Dependencies between libraries are acyclic!
* Folders and files
    * A folder represents a module. 
    * A file contains either of the following:
        * a data structure, a collection of related data structures and basic functionality,
        * free functions that operate on data structures.
    * Dependencies between folders on the same level need to be acyclic.
    * Either all files in a directory depend on subdirectories in the same folder or subdirectories depend on files from the parent directory, but not both.
* Namespaces
    * CArL lives within the `carl` namespace.
    * Each library has its own sub-namespace, except `carl-common`, `carl-arithmetic`, `carl-formula`, `carl-extpolys`.
    * Auxiliary functions are in an appropriate sub-namespace.

### C++ features

* As of now, please stick to C++17 features.
* Use `enum class` instead of `enum`.