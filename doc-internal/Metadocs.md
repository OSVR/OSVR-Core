# Documentation on Writing Documentation {#InternalMetadocs}

## Organization
There are two sets of documentation built:

- "Internal" - Also known as "implementation" docs, the target audience for this is someone developing the implementation of OSVR: someone working on this actual codebase, not just using it. If you're seeing this in Doxygen docs (rather than in a Git repo), you're looking at the internal docs.
	- Indexes `include` and `src`, as well as `examples`
	- Includes undocumented members and internal classes, and issues warnings for incomplete docs, which will show up on Jenkins.
	- Defines the preprocessor macro `OSVR_DOXYGEN`
- "External" - The target audience for this is for a _user_ of the OSVR framework - someone consuming presumably either ClientKit or PluginKit. As such, this documentation set only indexes the examples and the `inc/` directory, and excludes undocumented members (!). It also excludes a handful of documentation pages, including this one.
	- It defines the preprocessor symbol `OSVR_DOXYGEN_EXTERNAL` as well as `OSVR_DOXYGEN`

## Tools
- MSC (Message Sequence Chart) Diagrams
	- Canonical tool and syntax reference: [mscgen](http://www.mcternan.me.uk/mscgen/)
	- IDE-like tool for creating MSC diagrams: <http://sourceforge.net/projects/msc-generator/>
		- Note that not all constructs supported by `mscgen` are supported in this app
- Markdown (`.md`) text files
	- Can just use a text editor, many have support for syntax highlighting Markdown.
	- Windows app with good preview (except of course for Doxygen-specific extensions): [MarkPad](http://code52.org/DownmarkerWPF/)

## Tips

- To be able to make a nicely structured doc tree, include something like `{#` followed by some slug like the file basename followed by `}` in the first (heading) line of any Markdown file. Then, go into `doc/Manual.dox` and add it as a subpage in the appropriate place.

## Source Code
You can use `#ifndef OSVR_DOXYGEN_EXTERNAL` and `#endif` to hide chunks of implementation details in headers from Doxygen's external build. This is particularly useful for the C++ wrappers for ClientKit and PluginKit, since they by definition have to have all their innards exposed in the headers.

For public headers, it is useful to actually fill out the brief description at the top of the file (not just leave it saying `Header` and nothing more), since it will show up in the file list in Doxygen. Don't repeat the whole contents of the file, but give an idea of why someone might care about that file.