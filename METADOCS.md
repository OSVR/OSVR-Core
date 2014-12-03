# Documentation on Writing Documentation {#METADOCS}

## Tools
- MSC (Message Sequence Chart) Diagrams
	- Canonical tool and syntax reference: [mscgen](http://www.mcternan.me.uk/mscgen/)
	- IDE-like tool for creating MSC diagrams: <http://sourceforge.net/projects/msc-generator/>
		- Note that not all constructs supported by `mscgen` are supported in this app
- Markdown (`.md`) text files
	- Can just use a text editor, many have support for syntax highlighting Markdown.
	- Windows app with good preview (except of course for Doxygen-specific extensions): [MarkPad](http://code52.org/DownmarkerWPF/)

## Tips
- To be able to make a nicely structured doc tree, include something like `{#` followed by the file basename followed by `}` in the first (heading) line of any Markdown file. Then, go into `doc/Manual.dox` and add it as a subpage in the appropriate place.