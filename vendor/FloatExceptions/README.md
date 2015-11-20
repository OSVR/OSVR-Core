# FloatExceptions project

This is sample code downloaded from Bruce Dawson's blog post at <http://randomascii.wordpress.com/2012/04/21/exceptional-floating-point/> about floating point exceptions. The `.cpp` file contains all the snippets embedded in the blog post, along with a copyright statement and the [MIT license](http://www.opensource.org/licenses/mit-license) (and a link to the blog post).

The following files are original and unmodified (except perhaps by Git which might change line endings) from the sample code zip:

- `FloatExceptions.cpp` - original source
- `FloatExceptions.sln` - VS2010 solution file
- `FloatExceptions.vcxproj` - VS2010 project file
- `FloatExceptions.vcxproj.filters` - VS2010 project-related file

The following files have been added:

- `README.md` - this file
- `LICENSE.txt` - The copyright notice from `FloatExceptions.cpp`, with leading `// ` removed

The following additional files are excerpts of `FloatExceptions.cpp`, with tidy header dressing (include guards, includes, license and attribution boilerplate, etc.) around them and dummy do-nothing implementations for non-MSVC compilers. They are named for the single class they contain.

- `FPExceptionDisabler.h`
- `FPExceptionEnabler.h`


Ryan Pavlik, Sensics, Inc. - October 27, 2015
