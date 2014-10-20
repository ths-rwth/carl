# Operators {#polynomials_operators}

The classes used to build polynomials are fully compatible with respect to the following operators, that means that any two objects of these types can be combined if there is a directed path between them within the class hierarchy.
All the operators have the usual meaning.

- `operator==(lhs, rhs)`
- `operator!=(lhs, rhs)`
- `operator<(lhs, rhs)`
- `operator<=(lhs, rhs)`
- `operator>(lhs, rhs)`
- `operator>=(lhs, rhs)`
- `operator+(lhs, rhs)`
- `operator+=(lhs, rhs)`
- `operator-(lhs, rhs)`
- `operator-(rhs)`
- `operator-=(lhs, rhs)`
- `operator*(lhs, rhs)`
- `operator*=(lhs, rhs)`

We now give the result type for any combination of these types for all supported operators.

Note that some operators may throw an assertion when called with UnivariatePolynomial objects that have incompatible main variables.

### operator-(lhs) (unary minus)
-  | V  | M  | T  | MP | UP
-- | -- | -- | -- | -- | --
-  | T  | T  | T  | MP | UP

### operator+(lhs, rhs)
+  | V  | M  | T  | MP | UP
-- | -- | -- | -- | -- | --
V  | MP | MP | MP | MP | UP
M  | MP | MP | MP | MP | UP
T  | MP | MP | MP | MP | UP
MP | MP | MP | MP | MP | MP
UP | UP | UP | UP | MP | UP

### operator-(lhs, rhs)
-  | V  | M  | T  | MP | UP
-- | -- | -- | -- | -- | --
V  | MP | MP | MP | MP | UP
M  | MP | MP | MP | MP | UP
T  | MP | MP | MP | MP | UP
MP | MP | MP | MP | MP | MP
UP | UP | UP | UP | MP | UP

### operator*(lhs, rhs)
*  | V  | M  | T  | MP | UP
-- | -- | -- | -- | -- | --
V  | M  | M  | T  | MP | UP
M  | M  | MP | MP | MP | UP
T  | MP | MP | MP | MP | UP
MP | MP | MP | MP | MP | MP
UP | UP | UP | UP | MP | UP

## Implementation
We follow a few rules when implementing these operators:
- Of the comparison operators, only `operator==` and `operator<` contain a real implementation. The others are implemented like this:
  - `operator!=(lhs, rhs)`: `!(lhs == rhs)`
  - `operator<=(lhs, rhs)`: `!(rhs < lhs)`
  - `operator>(lhs, rhs)`: `rhs < lhs`
  - `operator>=(lhs, rhs)`: `rhs <= lhs`
- Of all `operator==`, only those where `lhs` is the most general type contain a real implementation. The others are implemented like this:
  - `operator==(lhs, rhs)`: `rhs == lhs`
- They are ordered like in the list above.
- Operators belong to the most general type involved and are implemented as `friend` methods, whether they need this or not.
- Overloaded versions of the same operator are ordered in decreasing lexicographical order, like in this example:
  - `operator(Term,Term)`
  - `operator(Term,Monomial)`
  - `operator(Term,Variable)`
  - `operator(Term,Coefficient)`
  - `operator(Monomial,Term)`
  - `operator(Variable,Term)`
  - `operator(Coefficient,Term)`
- Other versions, for example for comparisons with the coefficient type, are below those.

## Testing the operators
There are two stages for testing these operators: a syntactical check that these operators exist and have the correct signature and a semantical check that they actually work as expected.

### Syntactical checks
The syntactical check for all operators specified here is done in `tests/core/Test_Operators.cpp`.
We use `boost::concept_check` to check the existence of the operators. There are the following concepts:

- `Comparison`: Checks for all comparison operators. (`==`, `!=`, `<`, `<=`, `>`, `>=`)
- `Arithmetic`: Checks for all out-of-place arithmetic operators. (`+`, unary and binary `-`, `*`)
- `InplaceAddition`: Checks for all in-place addition operators. (`+=`, `-=`)
- `InplaceMultiplication`: Checks for all in-place multiplication operators. (`*=`)

### Semantical checks
Semantical checking is done within the test for each class.