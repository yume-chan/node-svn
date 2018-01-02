# node-svn

[![Maintenance](https://img.shields.io/maintenance/yes/2018.svg)]()

**Work In Progress**

Wrap SVN to Node Native Addon.

- [node-svn](#node-svn)
    - [build-svn branch](#build-svn-branch)
    - [Platform table](#platform-table)
    - [Dependencies](#dependencies)
    - [Requirements](#requirements)
    - [Patches](#patches)
    - [Building](#building)
    - [Generate patch file on Windows](#generate-patch-file-on-windows)
    - [Docs](#docs)
    - [Thread safety](#thread-safety)
    - [Roadmap](#roadmap)
    - [License](#license)

## build-svn branch

This branch contains source codes for all dependencies.

The `scripts/configure.js` script will configure them into `binding.gyp`.

All dependencies will be built as static library, and linked into a single dynamic library.

In this way, on Windows it won't use dlls from other SVN installations, on Linux the binrary doesn't need to be patched to change their library searching path.

## Platform table

|         | x86                                                                                                                                                                            | x64                                                                                                                                                                            |
| ------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| Windows | [![Build status](https://ci.appveyor.com/api/projects/status/u7klnu47dxei6w0x/branch/build-svn?svg=true)](https://ci.appveyor.com/project/yume-chan/node-svn/branch/build-svn) | [![Build status](https://ci.appveyor.com/api/projects/status/u7klnu47dxei6w0x/branch/build-svn?svg=true)](https://ci.appveyor.com/project/yume-chan/node-svn/branch/build-svn) |
| Linux   | No                                                                                                                                                                             | No                                                                                                                                                                             |
| macOS   | No                                                                                                                                                                             | No                                                                                                                                                                             |

**Help wanted!** I have no idea how to configure it on Linux and macOS, they may both require a complex configure script.

## Dependencies

| Name                   | Introduction                                      | Required by           | Note                                                                      |
| ---------------------- | ------------------------------------------------- | --------------------- | ------------------------------------------------------------------------- |
| node-apr-gen-test-char | A node wrapper for apr's gen-test-char executable | apr                   | Used to generate a header file (`apr_escape_test_char.h`) required by apr |
| zlib                   | ZLib compression library                          | apr, serf, subversion | See below                                                                 |
| expat                  | An XML parser                                     | apr                   |                                                                           |
| openssl                | crypto library                                    | serf                  | See below                                                                 |
| sqlite-amalgamation    | SQLite database                                   | subversion            | Unofficial mirror for amalgamation version                                |
| apr                    | Apache Portable Runtime                           | subversion            | apr-util has been merged into apr-2                                       |
| serf                   | An HTTP client library                            | subversion            |                                                                           |
| subversion             | Subversion library                                | node-svn              | Only static libraries are used                                            |

**Note for ZLib:**

Node.js and Electron all contain ZLib symbols, so this library can dynamic link to `node.lib` or `io.lib` to use ZLib.

But subversion's configuration script need ZLib, so a copy of ZLib is included.

**Note for OpenSSL:**

For Node.js, `node.lib` contains OpenSSL symbols, so this library can dynamic link to `node.lib` to use OpenSSL.

For Electron, `io.lib` doesn't contain OpenSSL symbols (See [this blog post](https://electronjs.org/blog/electron-internals-using-node-as-a-library#shared-library-or-static-library) for more information). So this library need to compile OpenSSL by itself.

The source code in `dependencies/openssl` folder is taken from [nodejs/node repository](https://github.com/nodejs/node/tree/master/deps/openssl), with a modified `openssl.gyp` to generate static library.

## Requirements

| Name   | Usage                             |
| ------ | --------------------------------- |
| Python | Generate subversion project files |

## Patches

1. Patch subversion configuration script to support new apr-util header format from apr-2.
1. Patch subversion configuration script to change the hardcoded expat searching path.
2. Patch subversion SQLite initialization to use **Serialize** mode (see [Thread Safety](#Thread-safey)).
1. Patch libexpat to include `expat.h` in package.
1. Patch serf to support new version of OpenSSL (with `OPENSSL_NO_DEPRECATED` on)

## Building

```` shell
# Clone
git clone -b build-svn https://github.com/yume-chan/node-svn.git
cd node-svn

# Clone and patch submodules
git submodule update --init
git apply --directory=dependencies/subversion ./dependencies/patches/subversion.diff
git apply --directory=dependencies/libexpat ./dependencies/patches/libexpat.diff
git apply --directory=dependencies/serf ./dependencies/patches/serf.diff

# Generate subversion project files
cd dependencies/subversion
python gen-make.py --with-zlib=../zlib --with-apr=../apr --with-apr-util=../apr --with-apr-iconv=../apr-iconv --with-sqlite=../sqlite-amalgamation --disable-shared --with-static-apr --vsnet-version=15 --disable-gmock --with-serf=../serf
cd ../..

# Build
npm install

# Tests
npm test
````

## Generate patch file on Windows

````shell
cd dependencies/subversion
git diff --no-color | Out-File -Encoding utf8 ../patches/subversion.diff
````

**Then change the line ending to LF in an editor!**

## Docs

See the [type definition](scripts/index.d.ts)

## Thread safety

SVN uses SQLite for databasing, SQLite has three different threading modes: (Original Documentation [here](https://sqlite.org/threadsafe.html))

1. **Single-thread**. In this mode, all mutexes are disabled and SQLite is unsafe to use in more than a single thread at once.
1. **Multi-thread**. In this mode, SQLite can be safely used by multiple threads provided that no single database connection is used simultaneously in two or more threads.
1. **Serialized**. In serialized mode, SQLite can be safely used by multiple threads with no restriction.

Orignally, SVN uses **Multi-thread** mode, means you *can* use a single `AsyncClient` to access multiple repositories concurrently, but *can not* apply multiple operations (even all of them are reading) to a single repository at the same time.

To make things worse, SQLite will just throw access violation when you try to do this, cause your program to crash.

This project includes a patch to use **Serialized** mode instead, so you should be free to do any operations to any repositories as you want.

## Roadmap

- [ ] Add options to all methods
- [ ] Strongly-typed error handling
- [ ] Iterator/Async Iterator for things like `status()`
- [ ] Authentication
- [ ] Cross platform
- [x] Better multi-threading handling
- [ ] You name it

**Help wanted!** How to create custom Error class in Node.js Native Addon?

## License

| Name                   | License                                                                            | Note                                                               |
| ---------------------- | ---------------------------------------------------------------------------------- | ------------------------------------------------------------------ |
| node-apr-gen-test-char | [MIT](https://github.com/yume-chan/node-apr-gen-test-char/blob/master/LICENSE)     |                                                                    |
| zlib                   | [zlib](https://github.com/madler/zlib/blob/master/zlib.h)                          |                                                                    |
| openssl                | [OpenSSL](https://github.com/openssl/openssl/blob/master/LICENSE)                  | Node.js: [MIT](https://github.com/nodejs/node/blob/master/LICENSE) |
| sqlite-amalgamation    | [BSD-3-clause](https://github.com/azadkuh/sqlite-amalgamation/blob/master/LICENSE) | SQLite: [Public Domain](http://www.sqlite.org/copyright.html)      |
| apr                    | [Apache 2.0](https://github.com/apache/apr/blob/trunk/LICENSE)                     |                                                                    |
| serf                   | [Apache 2.0](https://github.com/apache/apr/blob/trunk/LICENSE)                     |                                                                    |
| subversion             | [Apache 2.0](https://github.com/apache/subversion/blob/trunk/LICENSE)              |                                                                    |
| node-svn               | [MIT](https://github.com/yume-chan/node-svn/blob/master/LICENSE)                   |                                                                    |
