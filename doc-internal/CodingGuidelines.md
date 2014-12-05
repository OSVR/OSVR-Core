# Coding Guidelines/Style {#InternalCodingGuidelines}

The book ["C++ Coding Standards"](http://www.gotw.ca/publications/c++cs.htm) by Herb Sutter and Andrei Alexandrescu is considered the base standard, except where specifically indicated otherwise here.
The [LLVM coding standards](http://llvm.org/docs/CodingStandards.html) are potential sources of additional knowledge (for instance, allowable C++11 features for a given subset of compilers) but not binding.

## Additional Sources of Wisdom
- The [ISO C++ wiki's Style and Technique FAQ](http://isocpp.org/wiki/faq/style-and-techniques) (and the rest of the wiki too)
- Herb Sutter's [Elements of Modern C++ Style](http://herbsutter.com/elements-of-modern-c-style/)

## The Primary Rule

**Above all, be consistent with the code you’re working on.**

## Other Guidelines

Many of these will be automatic if you use the `clang-format` tool and follow The Primary Rule, especially if you're comfortable with modern idiomatic C++. Don't let the existence of these guidelines deter you from contributing, there's not a test at the end of this page.

1. Organization-specific

    1. All definitions intended to be exposed as C API and not part of a separable library should start with `osvr`

    2. The C++ namespace `osvr` should be used for C++ code not belonging to a separable library, with nested namespaces (all lowercase) for libraries within the core.

        1. Use named, anonymous, and `detail` nested namespaces as appropriate.

    3. Include files should be in a subdirectory appropriate to the library they belong to.

    4. Usage of C is limited to externally-facing APIs that require long term binary compatibility.
      Other code, including implementation of external APIs, should be in idiomatic, modern C++.

2. Naming convention and syntax

    1. Names of types (including `class`es, `struct`s and `typedef`s) should be in CamelCase (no leading `C` or `I` or other such decoration).

        1. If they are part of the C API, they should be prefixed with `OSVR_`

    2. Enumerations

        1. values should be in upper case with underscores, with the `OSVR_` prefix if part of a C API, and with some abbreviated prefix specific to that enumeration type.

        2. No anonymous enumerations.

        3. The enumeration type name should obey type rules -- see above.

    3. Functions should be camelCase (lowercase first letter)

        1.Note that in the C API, the `osvr` prefix is considered to be the first letter, so functions would look like `osvrMyFunction`

    4. Variables should be camelCase (lowercase first letter)

    5. Constant names shall be in capital letters.

    6. Class private members shall start with `m_`.

    7. Static data members shall start with `s_`, if they cannot be avoided, but work hard to avoid them.

    8. No global variables or enforced singletons (and static data members or function-static variables count as globals!) -- use dependency injection.

    9. C preprocessor macros are in all caps, prefix with `OSVR_` and with words delimited by underscores.
       (See below for more on macros.)

3. Formatting

    1. Use *clang-format* to automatically format your code: run it frequently during editing and before committing.
       (Windows binaries here: <http://llvm.org/builds/> )

    2. Use an existing pre-defined format included with *clang-format*, with minimal changes as needed.
       For instance, LLVM-style with auto detection of C++ dialect, 4-space indents, and inner namespace indentation.
       Existing projects already include the appropriate `.clang-format` file.

4. Coding Style

    1. For integer types, use specific width typedefs, except when working with code where some other specific type, e.g. `size_t` would be natural.

        1. `<stdint.h>` is missing/unusable before VS 2012, so include `<osvr/Util/StdInt.h>` instead: it will forward to the system header if usable.

    2. If a class is to be considered non-copyable, indicate by use of `boost::noncopyable`.
       Note, however, that with C++11 returning by value is fast, so think about the semantics of making something an object/reference type vs. a value type.

    3. API header files should be kept clean, without unnecessary includes or implementation.
       C++ header files are permitted to have trivial constructors.
       Virtual destructors, if any, must be in the `.cpp` file.

    4. Every if statement shall be followed by braces.

    5. Every exposed API shall have Doxygen comments, and other methods should have Doxygen comments where the name is not self-explanatory. Use the "at" form of Doxygen commands (e.g., <code>\@brief</code>) rather than the "backslash form" (e.g., <code>\\brief</code>)

    7. Mark todos in the code with <code>/// \@todo some note</code> so that Doxygen and other tools can pick it up.

    8. Raw pointers in C++ code are permitted very rarely.
       Smart pointers should usually be used, and where ownership is not being transferred or shared, passing by (const) reference typically suffices and describes/enforces non-null in the interface.

        1.  Prefer `unique_ptr` (include `<osvr/Util/UniquePtr.h>` to get it in the `osvr` namespace) to `shared_ptr` (likewise, `<osvr/Util/SharedPtr.h>`)

    9. Avoid C preprocessor macros when possible (don’t use them as inline functions or typedefs, OK to use them for code generation purposes), and `#undef` ‘ed if possible after they are done being used to avoid polluting the global namespace.