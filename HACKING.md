# Guidelines for Development/Contributing.

Please use `clang-format` to keep the code tidy. The `.clang-format` 
config file is in the root of the repository. We're basically using the 
LLVM style, except with 4-space indentation, inner namespace indentation,
and C++03 (since turning on C++11 in `clang-format` sometimes makes things
not work in less-compelling compilers).

If you're using Windows, please install a recent [llvm/clang 
binary][llvmwin], and choose to add the directory to the path. (Makes it 
easier.) If you've got a non-Express version of Visual Studio, you might 
also want the clang-format plugin. In either case, there are a pair of 
scripts in `/devtools/`: 

- `format-project.sh` runs `clang-format` in place using unix `find` to 
    hopefully grab every file. Windows users need Git for Windows installed
    and handling `.sh` files to run this. (A default install will do that, IIRC)

- `format-file.cmd` runs `clang-format` in place on whatever file or 
    files you drop on it - useful if you want some other code to also look 
    in the same style. 

[llvmwin]:http://llvm.org/builds/

## Naming Conventions

### General
- Macros, constants, and enum values are all caps.

- The "body" of a type name is in CamelCase.

- Functions are camelCase except for the first character.

- No Hungarian notation - the closest we come is `m_whatever` for
    private class members.

- Be consistent and match surrounding code.

### C Code (APIs only)
- Functions should be prefixed by `ogvr` - this means a function is 
    named like `ogvrMyFunction` 

- Types (typedefs, structs) should be prefixed by `OGVR_` - this means a 
    type is named like `OGVR_MyType` 

- Macros, constants, and enum values should also be prefixed by `OGVR_`

### C++ Code
- Use lowercase namespaces such as `ogvr`, nested appropriately as required.

- Do not add prefixes (`C`, `I`, `ogvr`, `OGVR`, etc) to the beginning 
    of types or functions - those are either Hungarian notation or made 
    redundant by C++'s support for namespaces. 


