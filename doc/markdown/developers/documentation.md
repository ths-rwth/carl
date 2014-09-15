Documentation {#documentation}
==============================

On this page, we refer to some internal documentation rules.
We use doxygen to generate our documentation and code reference.
The most important conventions for documentation in CArL are collected here.

### Modules
In order to structure the reference, we use the concept of
[Doxygen modules](http://www.stack.nl/~dimitri/doxygen/manual/grouping.html#modules).
Such modules are best thought of as a hierarchical set of tags, called groups. 
We define those groups in `/doc/markdown/codedocs/groups.dox`.
Please make sure to put new files and classes in the appropriate groups.

### Literature references
Literature references should be provided when appropriate.

We use a bibtex database located at `/doc/literature.bib` with the following conventions:

- Label for one author: `LastnameYY`, for example `Ducos00` for @cite Ducos00.
- Label for multiple authors: `ABCYY` where `ABC` are the first letters of the authors last names. For example `GCL92` for @cite GCL92.
- Order the bibtex entrys by label.

These references can be used with `@cite label`, for example like this:
@code
/*/** */*
 * Checks whether the polynomial is unit normal
 * @see @cite GCL92, page 39
 * @return If polynomial is normal.
 */
bool isNormal() const;
@endcode 

### Code comments


- File headers
@code
/*/** */*
 * @file <filename>
 * @ingroup <groupid1>
 * @ingroup <groupid2>
 * @author <author1>
 * @author <author2>
 * 
 * [ Short description ]
 */
@endcode

Descriptions may be omitted when the file contains a single class, either implementation or declaration.


- Namespaces are documented in a separate file, found at '/doc/markdown/codedocs/namespaces.dox'

- Class headers
@code
/*/** */*
 * @ingroup <groupid>
 * [ Description ]
 * @see <reference>
 * @see <OtherClass>
 */
@endcode

- Method headers
@code
/*/** */*
 * [ Usage Description ]
 * @param <p1> [ Short description for first parameter ] 
 * @param <p2> [ Short description for second parameter ]
 * @return [ Short description of return value ]
 * @see <reference>
 * @see <otherMethod>
 */
@endcode

These method headers are written directly above the method declaration. 
Comments about the implementation are written above the or inside the implementation. 

The `see` command is used likewise as for classes.

### Writing out-of-source documentation

Documentation not directly related to the source code is written in Markdown format, and is located in
'/doc/markdown/'.
