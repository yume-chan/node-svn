# node-svn

[![Maintenance](https://img.shields.io/maintenance/yes/2018.svg)]()

**Work In Progress**

Wrap Subversion (SVN) to Node Native Addon

- [node-svn](#node-svn)
    - [Platform table](#platform-table)
    - [Dependencies](#dependencies)
    - [Patches](#patches)
    - [Building](#building)
    - [Docs](#docs)
    - [Thread safety](#thread-safety)
    - [Roadmap](#roadmap)
    - [License](#license)

## Platform table

|         | Build                      | Tests                     |
|---------|----------------------------|---------------------------|
| Windows | [![AppVeyor Build][1]][2]  | [![AppVeyor Tests][5]][2] |
| Linux   | [![Travis CI Build][3]][4] |                           |
| macOS   | [![Travis CI Build][3]][4] |                           |

[1]: https://img.shields.io/appveyor/ci/yume-chan/node-svn.svg
[2]: https://ci.appveyor.com/project/yume-chan/node-svn
[3]: https://img.shields.io/travis/yume-chan/node-svn.svg
[4]: https://travis-ci.org/yume-chan/node-svn
[5]: https://img.shields.io/appveyor/tests/yume-chan/node-svn.svg

## Dependencies

| Name                   | Introduction                                      | Required by | Note                                                                      |
| ---------------------- | ------------------------------------------------- | ----------- | ------------------------------------------------------------------------- |
| node-apr-gen-test-char | A node wrapper for apr's gen-test-char executable | apr         | Used to generate a header file (`apr_escape_test_char.h`) required by apr |
| expat                  | An XML parser                                     | apr         |                                                                           |
| openssl                | crypto library                                    | serf        | *See below*                                                               |
| sqlite-amalgamation    | SQLite database                                   | subversion  | Unofficial mirror for amalgamation version                                |
| apr                    | Apache Portable Runtime                           | subversion  | apr-util has been merged into apr-2                                       |
| serf                   | An HTTP client library                            | subversion  |                                                                           |
| subversion             | Subversion library                                | node-svn    | Only static libraries are used                                            |

**Note for OpenSSL:**

For Node.js, `node.lib` exports OpenSSL symbols, so this library can dynamic link to `node.lib` to use OpenSSL.

For Electron, `io.lib` doesn't export OpenSSL symbols (See [this blog post](https://electronjs.org/blog/electron-internals-using-node-as-a-library#shared-library-or-static-library) for more information). So this library need to compile OpenSSL by itself.

The source code in `dependencies/openssl` folder is taken from [nodejs/node repository](https://github.com/nodejs/node/tree/master/deps/openssl), with a modified `openssl.gyp` to build as static library.

## Patches

2. Patch subversion SQLite initialization to use **Serialize** mode (see [Thread Safety](#Thread-safey)).
1. Patch serf to support new version of OpenSSL (with `OPENSSL_NO_DEPRECATED` on)

## Building

```` shell
# Clone
git clone -b build-svn https://github.com/yume-chan/node-svn.git
cd node-svn

# Clone and patch submodules
git submodule update --init

# Build
npm install

# Tests
npm test
````

It will takes minutes to build (~120s on my i7-7567U 2C4T 4GHz, 8G DDR4 2133MHz, ~250s if openssl included), so sit back and relax yourself while it's doing its own things.

All dependencies will be compiled as static library and be linked into one single dynamic library to avoid any dynamic library searching path problems.

## Docs

See the [type definition](scripts/index.d.ts)

## Thread safety

Svn has been designed to only work in single-thread mode. So you need to create new `Client` for each concurrency operatons.

Maybe a Client pool can be used to reduce overhead.

*I have tried to enable all mutex in SQLite, but svn still crashes with random access violations.*

## Roadmap

- [ ] Add options to all methods
- [ ] Strongly-typed error handling
- [x] Async Iterator for methods like `status()`
- [x] Authentication
- [ ] Cross platform
- [ ] You name it

**Help wanted!** How to create custom Error class in Node.js Native Addon?

## License

| Name                   | License                                                                            | Note                                                               |
| ---------------------- | ---------------------------------------------------------------------------------- | ------------------------------------------------------------------ |
| node-apr-gen-test-char | [MIT](https://github.com/yume-chan/node-apr-gen-test-char/blob/master/LICENSE)     |                                                                    |
| openssl                | [OpenSSL](https://github.com/openssl/openssl/blob/master/LICENSE)                  | Node.js: [MIT](https://github.com/nodejs/node/blob/master/LICENSE) |
| sqlite-amalgamation    | [BSD-3-clause](https://github.com/azadkuh/sqlite-amalgamation/blob/master/LICENSE) | SQLite: [Public Domain](http://www.sqlite.org/copyright.html)      |
| apr                    | [Apache 2.0](https://github.com/apache/apr/blob/trunk/LICENSE)                     |                                                                    |
| serf                   | [Apache 2.0](https://github.com/apache/apr/blob/trunk/LICENSE)                     |                                                                    |
| subversion             | [Apache 2.0](https://github.com/apache/subversion/blob/trunk/LICENSE)              |                                                                    |
| node-svn               | [MIT](https://github.com/yume-chan/node-svn/blob/master/LICENSE)                   |                                                                    |
