# node-svn

**Work In Progress**

Wrap SVN to Node Native Addon.

- [node-svn](#node-svn)
    - [Platform](#platform)
    - [Installation](#installation)
    - [API](#api)
    - [Update SVN](#update-svn)
        - [Windows](#windows)
        - [Linux](#linux)
    - [Building](#building)
        - [Visual Studio](#visual-studio)
        - [node-gyp](#node-gyp)
    - [Debugging](#debugging)
    - [Multi-threading](#multi-threading)
    - [Roadmap](#roadmap)
    - [vscode-svn](#vscode-svn)

## Platform

It relies on pre-built SVN shared library binaries. Here is the platform support table.

|      | Windows                         | Linux            | macOS |
| ---- | ------------------------------- | ---------------- | ----- |
| x86  | Yes (not tested)                | No               | No    |
| x64  | Yes                             | Yes (not tested) | No    |
| Note | Build with Visual Studio 15.4.4 | Build with gcc-7<br> `patchelf` also required | N/A   |

It uses many C++17 features so an older compiler may not work.

## Installation

You need at least Node.js v8.5.0

It's not published to npm yet, so just install it by

````shell
npm install yume-chan/node-svn
````

## API

See [the type definition](scripts/index.d.ts).

## Update SVN

### Windows

I uses TortoiseSVN on Windows.

1. `svn checkout svn://svn.code.sf.net/p/tortoisesvn/code/trunk tortoisesvn-code`
1. Install dependencies:
    * Visual Studio 2017
    * Java
    * Python 2.6 x86
    * libxml2 Python bindings
    * Perl
    * Wix
    * NAnt
1. Copy `default.build.user.tmpl` to `default.build.user`
1. Run `nant init`
1. To compile 32-bit SVN:
    * Launch `x86 Native Tools Command Prompt for VS 2017`
1. To compile 64-bit SVN:
    * Launch `x64 Native Tools Command Prompt for VS 2017`
1. `cd` to working directory
1. Run `nant Subversion` to compile
    > Note: If you don't have Windows 8.1 SDK installed, nant will fail building `CrashServer` project, that's fine.
1. Enter `ext` directory
1. Enter `apr/release_${arch}` directory
    1. Copy all items in `include` directory to `node-svn/include/win32/apr`
    1. Copy `libapr_tsvn.dll`, `libapr_tsvn.lib` and `libapr_tsvn.pdb` to `node-svn/lib/svn/win32/${arch}`
1. Enter `apr-util/release_${arch}` directory
    1. Copy `libaprutil_tsvn.dll` and `libaprutil_tsvn.pdb` to `node-svn/lib/svn/win32/${arch}`
1. Enter `cyrus-SASL/release_${arch}` directory
    1. Copy all `.dll` and `.pdb` to `node-svn/lib/svn/win32/${arch}`
1. Enter `Subversion/release_${arch}` directory
    1. Copy `libsvn_tsvn.dll`, `libsvn_tsvn.lib` and `libsvn_tsvn.pdb` to `node-svn/lib/svn/win32/${arch}`

### Linux

1. Download and unzip APR and APR-util source code from [https://apr.apache.org/download.cgi](https://apr.apache.org/download.cgi)
1. Download and unzip SVN source code from [https://subversion.apache.org/download.cgi?update=201708081800](https://subversion.apache.org/download.cgi?update=201708081800)
1. Download dependencies and unzip them into SVN source directory
    * zlib
    * sqlite-amalgamation
1. Install dependencies
    * autoconf
    * libtools
1. Enter APR source directory
    1. `./configure`
        > If there is an error about `expat`, install `expat` into your system
    1. `make`
    1. `make install`
1. Enter APR-util source directory
    1. `./configure`
        > If there is an error about apr not found, use `./configure --with-apr=/usr/local/apr` instead.
    1. `make`
    1. `make install`
1. Enter SVN source directory
    1. `./configure`
        > If there is an error about apr and apr-util not found, use `./configure --with-apr=/usr/local/apr --with-apr-util=/usr/local/apr` instead.
    1. `make`
    1. `make install`
1. Enter `usr/local/apr`
1. Copy all files in `include/apr-1` to `node-svn/include/linux/apr`
1. Copy `libapr-1.so` and `libaprutil-1.so` from `lib` to `node-svn/lib/svn/linux/x64`
1. Enter `usr/local/`
1. Copy all files in `include/subversion-1` to `node-svn/include/linux/svn`
1. Copy `libsvn_*.so` from `lib` to `node-svn/lib/svn/linux/x64`

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
- [ ] Iterator/Async Iterator for things like `status()`
- [ ] Authentication
- [ ] Cross platform
- [ ] Better multi-threading handling
- [ ] You name it

## vscode-svn

This package is used by my [vscode-svn](https://www.github.com/yume-chan/vscode-svn) project, which is an extension for Visual Studio Code.
