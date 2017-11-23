# node-svn

**Work In Progress**

Wrap SVN to Node Native Addon.

- [node-svn](#node-svn)
    - [Platform](#platform)
    - [Installation](#installation)
    - [API](#api)
    - [Dependencies](#dependencies)
    - [Building](#building)
        - [Visual Studio](#visual-studio)
        - [node-gyp](#node-gyp)
    - [Debugging](#debugging)
    - [Multi-threading](#multi-threading)
    - [Roadmap](#roadmap)
    - [vscode-svn](#vscode-svn)

## Platform

It relies on pre-built SVN shared library binaries.

|     | Windows          | Linux | macOS |
| --- | ---------------- | ----- | ----- |
| x86 | Yes (not tested) | No    | No    |
| x64 | Yes              | No    | No    |

## Installation

It's not published to npm yet, so just install it by

````shell
npm install yume-chan/node-svn
````

## API

See [the type definition](scripts/index.d.ts).

## Dependencies

This repository contains SVN header files and pre-built shared libraries for Windows x86 and x64, from the SVN repository.

To update it, following the [SVN building instruction](http://svn.apache.org/repos/asf/subversion/trunk/INSTALL) (for Windows, see [Update SVN on Windows](#updating-svn-on-windows) bolow) to build the binaries, then copy include and binaries to include and lib folders.

## Building

These is two build target: Visual Studio and node-gyp.

### Visual Studio

It requires Visual Studio 2017.

Visual Studio doesn't know where to find Node.js, `./scripts/update-node.ps1` will download Node.js header files and static libraries to use with Visual Studio.

1. Invoke `./scripts/update-node.ps1`
1. Open `node-svn.sln`
1. Press <kbd>Ctrl</kbd> + <kbd>Shift</kbd> + <kbd>B</kbd>

A post-build operation, `./scripts/postinstall.js` will copy SVN dependencies to the output folder.

### node-gyp

While installing this package from npm, node-gyp is used to configure and build.

When `npm install` is invoked, three steps defined in `package.json` will run:

1. `./scripts/preinstall.js` will generate `binding.gyp` from all `.cpp` files in `src` folder.
1. `./scripts/index.js` will invoke `node-gyp`.
1. `./scripts/postinstall.js` will copy SVN dependencies to the output folder.

## Debugging

You can use Visual Studio to debug. You must have the same version of Node as the bundled static library installed.

1. Edit your debug script in `/tests/index.js`.
1. Return to Visual Studio
1. Press <kbd>F5</kbd>

## Multi-threading

SVN uses SQLite for databasing, SQLite has three different threading modes: (Original Documentation [here](https://sqlite.org/threadsafe.html))

1. **Single-thread**. In this mode, all mutexes are disabled and SQLite is unsafe to use in more than a single thread at once.
1. **Multi-thread**. In this mode, SQLite can be safely used by multiple threads provided that no single database connection is used simultaneously in two or more threads.
1. **Serialized**. In serialized mode, SQLite can be safely used by multiple threads with no restriction.

Sadly, SVN uses **Multi-thread** and it cannot be changed, meaning you *can* use a single `AsyncClient` to access multiple repositories concurrently, but *can not* apply multiple operations (even all of them are reading) to a single repositories at the same time.

To make things worse, SQLite will just throw access violation when you try to do this, cause your program to crash, so don't do this.

## Roadmap

- [ ] Add options to all methods
- [ ] Strongly-typed error handling
- [ ] Async Iterator for things like `status()`
- [ ] Cross platform
- [ ] Better multi-threading handling
- [ ] You name it

## vscode-svn

This package is used by my [vscode-svn](https://www.github.com/yume-chan/vscode-svn) project, which is an extension for Visual Studio Code.
