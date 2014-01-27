Documentation {#documentation}
==============================

On this page, we refer to some internal documentation rules.
We use doxygen to generate our documentation and code reference.

## Code reference 

### Modules
In order to structure the reference, we use the concept of
[Doxygen modules](http://www.stack.nl/~dimitri/doxygen/manual/grouping.html#modules).
Such modules are best thought of as a hierarchical set of tags, called groups. 
We define those groups in `/doc/markdown/codedocs/groups.dox`.
Please make sure to put new files and classes in the appropriate groups.

### Literature references
Although doxygen provides the `cite` command, we use `see` to provide a reference,
as this is more flexible (it allows to add some additional information, like the correct page, as well.

New references should be added in `/doc/markdown/references.md`, where a full specification of authors and title should be found,
along with an anchor to provide hyperlinks.

### Code comments


- File headers
\verbatim
/**
 * @file <filename>
 * @ingroup <groupid1>
 * @ingroup <groupid2>
 * @author <author1>
 * @author <author2>
 * 
 * [ Short description ]
 */
\endverbatim

Descriptions may be omitted when the file contains a single class, either implementation or declaration.


- Namespaces are documented in a separate file, found at '/doc/markdown/codedocs/namespaces.dox'

- Class headers
\verbatim
/**
* @ingroup <groupid>
* [ Description ]
* @see <reference>
* @see <OtherClass>
*/
\endverbatim

- Method headers
\verbatim
/**
* [ Usage Description ]
* @param <p1> [ Short description for first parameter ] 
* @param <p2> [ Short description for second parameter ]
* @return [ Short description of return value ]
* @see <reference>
* @see <otherMethod>
*/
\endverbatim

These method headers are written directly above the method declaration. 
Comments about the implementation are written above the or inside the implementation. 

The `see` command is used likewise as for classes.

## Writing out-of-source documentation

Documentation not directly related to the source code is written in Markdown format, and is located in
'/doc/markdown/'.