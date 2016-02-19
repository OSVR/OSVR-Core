# OSVR Print Tree              {#OSVRPrintTree}

This is a simple but handy command-line tool for inspecting a core data structure of a running OSVR Server: the "path tree". As with most other OSVR tools, you can run with `--help` to get usage information.

Note: In order for OSVR Print Tree (or just Print Tree) to give you useful data, there has to be a server running for it to receive data from.

## Operation

OSVR Print Tree connects to your OSVR Server - if it has trouble connecting, it will wait until it successfully connects or you close the tool. Finally, it visits every node in the path tree in turn (in a depth-first traversal), and for most node types, prints some output to the console. The details of what output is printed are configurable.

> Technical note: Once it connects and receives the path tree message from the server, Print Tree performs all the processing that a client would do if it were to try to connect to every valid path, filling out the bare path tree received from the server to a "fully-resolved" path tree.  You're seeing a maximally-expanded path tree intended for human debugging, not anything relating to the wire format for sending path trees between client and server.

## General output format

~~~
[    AliasElement] /me/head
                    -> /com_osvr_Multiserver/OSVRHackerDevKitPrediction0/semantic/hmd
~~~

This is a sample output entry for a node: here, the `/me/head` node on an OSVR HDK using just orientation tracking. We'll break it down piece by piece.

- `[    AliasElement]` - All node output starts with a bracketed section that contains the node type name: here, it's `AliasElement`. These type names are all padded and right-aligned so that they're easy to skim in the output.
- `/me/head` - Immediately following the node type name on the same line is the full path of the node in the path tree.

In this case, there is another line after the path line for this node, giving additional information. What comes next after the path line can vary between node types and depending on your settings, but (with the exception of very long lines that your console may wrap for you) no text will come back and line up under the type name or the leading `/` of the path, to make it easy to skim and read. This is especially useful if you redirect the output to a text file and open it in a text editor with wordwrap turned off.

- `-> /com_osvr_Multiserver/OSVRHackerDevKitPrediction0/semantic/hmd` - Because of the settings used (see below) and because this is an alias node, Print Tree is showing that `/me/head` "points to" this other path. (You could look in the output for that path and see where it points, etc.) Note how it lines up past the leading slash of the `/me/head` path. Other node types may have other additional data, but it will show up in similar ways.

## Options

### Using the command line options

For every optional display `foo`, to turn it on/show it, you can do:

- `--foo 1` (which is how default values of true/show will show up in the `--help` output)
- `--foo true`
- `--show-foo` which is perhaps most useful but the most troublesome to shoehorn into that help screen.

And, to turn it off/hide it, you can do similarly:
- `--foo 0` (which is how default values of false/hide will show up in the `--help` output)
- `--foo false`
- `--hide-foo`

We'll just describe the `foo` options here and let you pick how you want to use them.

### Available options

- `alias-source` (default: show) - whether to show the source/target of an alias node: what the alias points to. Note that this just shows the first level into the alias, though a warning will be shown if the alias couldn't be fully resolved down to the device/interface/sensor level whether or not this option is enabled.
- `alias-priority` (default: hide) - whether to show the numeric priorities associated with aliases. These are primarily used by plugin developers in device descriptors: automatic aliases from descriptors have lower priorities than those set by the user in a config file by default so that a user can always override automatic configuration. This mostly operates behind the scenes without intervention, which is why it is hidden by default.
- `device-details` (default: show) - whether to show the "basic details" associated with a device node: the device instance name and server address/port (which can be useful when interoperating with other OSVR servers or VRPN clients).
- `device-descriptors` (default: hide) - whether to print the full JSON device descriptor associated with each device node. This is disabled by default simply because the device descriptors can be large text files, and for most tasks that Print Tree is used for, the relevant parts of the descriptors are already active and visible in their effects in the tree itself.
- `sensors` (default: show) - whether to show sensor nodes. If you have a large number of sensor nodes (for instance, in a skeleton/mo-cap situation), turning this off might help clear the noise from the output if you're looking for something specific that doesn't involve a sensor node.
- `string-data` (default: show) - whether to show the contents of so-called "string data" nodes (currently, the display descriptor and RenderManager configuration nodes are the most common ones). They are a somewhat large amount of output, but they are also frequently of interest when troubleshooting.
