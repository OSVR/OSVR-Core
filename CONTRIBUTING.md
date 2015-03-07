# Contributing Guidelines

Issues, pull requests, and other contributions are welcomed!

A few tips:

## Is `OSVR-Core` the right repository?

The OSVR system consists of a number of repositories, many of which are in the [OSVR organization on GitHub][osvr-org].

The `OSVR-Core` repo consists of the core (native-code) libraries forming the base of the OSVR system, including the ClientKit and PluginKit C APIs and the C++ header-only wrappers. A basic console-style server application (`osvr-server`), examples of both the plugin and client APIs, development utilities, and some development documentation targeted at C and C++ users round out the mix of development and runtime components. Finally, some useful "standard" plugins that are licensed under the same terms as the rest of OSVR-Core are also included.

- If you've got an issue to report, a bug fix, or a feature addition to those components: **yes**, you're in the right spot!
- If you would like to contribute a plugin that can be licensed the same as the rest of the OSVR-Core and can be built cross-platform without many extra dependencies: **most likely yes**, this is the right place - if it's not, we'll help you find the right place.
- If you've got a contribution to the OSVR-Unity integration, the OSVR-Unreal integration, or other game engine integration: **you're close** - see the [org page][osvr-org] for a listing of repositories to find the one for your game engine integration.
- If you've got a new game-engine integration: **start by filing an issue here**, we can get a repository set up for your new integration to move into.
- If you have a hardware or software issue related to OSVR but aren't sure exactly where it fits: **let us know with a support ticket at <http://support.osvr.com>**

[osvr-org]: https://github.com/osvr

## Getting ready

General "getting started" instructions can be found at <http://wiki.osvr.com>.

When making pull requests, please fork the project and create a topic branch off of the `master` branch.
(This is what GitHub does by default if you start editing with your web browser.)

When developing, make small commits that are nevertheless "whole": small enough to review, but each containing a logical single change in its entirety.
(If you don't understand what we mean by this, that's OK, we'll work it out.)

It's OK to rebase your topic branch to make the history more clear.
Avoid merging from master into your topic branch: if you need a change from master, rebase; otherwise, try to keep topic branches short-lived enough that we can get your code in to the mainline before much else changes!

Try to develop code that is portable (not particularly tied to a single operating system/compiler/etc) - OSVR runs on a number of platforms, and while we don't expect you to have all of them to test on, it's good to keep in mind. Our continuous integration server will be able to help with this.

If you're adding something reasonably testable, please also add a test.
If you're touching code that already has tests, make sure they didn't break.

There are code style guidelines - see the `[HACKING](hacking.md)` document for the details.
The main points are to match code surrounding what you're edited, and to be sure to use `clang-format`.
These help ensure that your changes are not artificially large because of whitespace, etc, that it's easy to review your changes, and that your code will be maintainable in the future.

Keep in mind that changes to PluginKit and ClientKit, particularly their headers, have the potential to impact (that is, break) a lot of downstream code (plugins, games), and break ABI, so changes in these areas will be more rigorously reviewed and discussed.
We'd recommend opening an issue for discussion before you start coding on these areas, to make sure we'll be able to accept your contribution.

## License

No formal copyright assignment is required. If you're adding a new file, make sure it has the appropriate license header. Any contributions intentionally sent to the project are considered to be offered under the license of the project they're sent to.

