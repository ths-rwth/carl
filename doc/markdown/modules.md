# CArL module structure {#modules}

CArL is separated into several libraries implementing functionality on different abstraction levels.

## General utilities

* carl-common: Basic data structures, helper methods, etc.
* carl-logging: Logging funtionality. *Depends on carl-common.*
* carl-statistics: Collect statistics about a run of a program. *Depends on carl-common.*
* carl-checkpoints: Collect the trace of the run of a programm and compare it with certain checkpoints. *Depends on carl-common and carl-logging.*
* carl-settings: Runtime settings infrastructure.

## Core libraries

*  carl-arithmetic: Arithmetic package. Does not do sophisticated memory management. *Depends on carl-common and carl-logging.*
* carl-formula: Logical formulas with support for arithmetic, bitvector and uninterpreted function constraints. Does pooling of some types for memory efficiency. *Depends on carl-arithmetic.*
* carl-vs: Implements virtual substitution. For legecy reasons, this depends on carl-formula.
* carl-extpolys: Extended polynomial types: factorized polynomials, rational functions. *Depends on carl-arithmetic.*

## Higher level

* carl-io: Input/output functionality for CArL types from/to different file formats. *Depends on carl-formula.*
* carl-covering: Data structures and heuristics for computing coverings. *Depends on carl-common and carl-logging.*