Nebula Code Guidelines
======================
C++ is the primary language used for the development of Nebula. While being
extremely powerful, the language has significant complexity which might turn the
code unmanageable, difficult to read or maintain. The goal of this set of rules
and guidelines is to manage this complexity by explicitly allowing or
disallowing certain features or practices.

Naming
======
As names are the most fundamental part of reading and understanding code, they
should be optimised for readability. Avoid using abbreviations or removing
letters from words. Proper nouns and universal abbreviations in names are
permitted to not adhere to the following rules if doing otherwise would hinder
readability.

Files
-----
File names are lowercase with underscores ``_`` separating individual words.

C++ source files should end in ``.cpp`` and header files should end in
``.hpp``.

Code
----
Names follow the Anton_Case convention:
- Words are separated with underscores ``_``.
- Variable, constant, function, namespace, typedef/using names are in
  lowercase.
- In class and enumerator names each word starts with an uppercase letter
  followed by lowercase letters.
- Macro names are all uppercase.

Formatting
==========
The project comes with a clang-format file to automatically impose a predefined
formatting.

Style
=====
#pragma guards
--------------
``#pragma`` guards are an alternative mechanism for preventing multiple
inclusion of a single header file in a TU.

**Pros:**
- Supported across all major compilers.
- Less verbose and error prone.

**Cons:**
- Non-standard solution.

**Decision:**
Use ``#pragma`` guards.

Use #include<>
--------------

Avoid the Standard Library
--------------------------
The Standard Library (STD) provides a variety of functions and utilities to ease
daily programming. However, STD:
- is extremely large and contains features far outside the scope of what
  is necessary,
- is extremely verbose and relies heavily on templates making it difficult
  to read,
- provides debugging utilities which result in the behaviour of the code
  changing between debug and release builds,
- is slow to compile,
- has inferior performance.
Thus it is best to avoid STD and use the available replacement
library.

Use struct instead of class
---------------------------

Use typename instead of class
-----------------------------

Use typed enums
---------------

Use static instead of unnamed namespaces
----------------------------------------

Use C++ casts
-------------
C++ introduced 4 intrinsic functions to cast types - ``static_cast``,
``const_cast``, ``reinterpret_cast`` and ``dynamic_cast``. Unlike
the C cast, those can perform only certain types of type conversions and will
generate errors when violating the rules, e.g. it is impossible to
``static_cast`` ``T const\*`` to ``T*``.

**Pros:**
- Clearer intent.
- Stricter casting rules.
- Error diagnostics.

**Cons:**
- More verbose.
- ``dynamic_cast`` is expensive as it traverses the inheritance
  hierarchy.
- ``dynamic_cast`` throws exceptions when it fails.

**Decision:**
Use C++ casts instead of C casts. Use ``reinterpret_cast`` judiciously. Do
not use ``dynamic_cast``.

Prohibited
==========
External Global Variables
-------------------------

Exceptions
----------
Do not use exceptions.

**Pros:**
- Exceptions are the only mechanism in the language that allows a
  constructor to fail. Without exceptions the work of a constructor must be
  moved to an initialisation function, but that requires an "invalid" state.

**Cons:**
- Throwing exceptions creates implicit exit points from functions
  complicating the control flow of the program and making it difficult to reason
  about the code as functions may exit in places you do not expect.
- Throwing a new exception silently changes the contract of the function
  forcing us to reexamine all transitive callers for exception safety.
- Compiling with exceptions embeds additional information into the binary
  file resulting in size increase.
- Exceptions are slow on the exceptional path.

**Decision:**
Exceptions and all related utilities provide benefits, however, the
aforementioned disadvantages greatly outweigh them. We do not believe that the
alternative mechanisms for handling error states, which are already in place,
introduce a significant burden.

RTTI
----
Run Time Type Information (RTTI)

Authors
=======
Piotr Kocia <peter.kocia@gmail.com>
