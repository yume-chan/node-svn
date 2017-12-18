# node-svn

## build-svn branch

This branch contains source codes for all dependencies.

The `scripts/configure.js` script will configure them into `binding.gyp`.

All dependencies will be built as static library, and link into a single dynamic link library contains.

In this way, on Windows it won't use dlls from other SVN installations, on Linux the binrary doesn't need to be patched to change their library searching path.

## Platform table

|     | Windows | Linux | macOS |
| --- | ------- | ----- | ----- |
| x86 | Yes     | No    | N/A   |
| x64 | Yes     | No    | No    |

## Dependencies

| Name                   | Introduction                                    | Usage      | Note                                                                             |
| ---------------------- | ----------------------------------------------- | ---------- | -------------------------------------------------------------------------------- |
| zlib                   | ZLib compression library                        | apr, serf  | node.lib includes dynamic version of zlib, so it is only used for configuration. |
| openssl                | crypto library                                  | apr, serf  | node.lib includes dynamic version of openssl                                     |
| node-apr-gen-test-char | A node wrapper for apr gen-test-char executable | apr        |                                                                                  |
| expat                  | An XML parser                                   | apr        |                                                                                  |
| apr                    | Apache Portable Runtime                         | subversion | apr-util has been merged into apr-2                                              |
| sqlite-amalgamation    | SQLite database                                 | subversion | Unofficial mirror for single file version                                        |
| serf                   | An HTTP client library                          | subversion |                                                                                  |
| subversion             | Subversion library                              | node-svn   | Only static libraries are used                                                   |

## Requirements

| Name   | Usage                             |
| ------ | --------------------------------- |
| Python | Generate subversion project files |

## Patches

1. Patch subversion configuration script to support new apr-util header format from apr-2.
1. Patch subversion configuration script to change the hardcoded expat searching path.

## Building

```` shell
# Clone
git clone -b build-svn https://github.com/yume-chan/node-svn.git
cd node-svn

# Clone and patch submodules
git submodule update --init
git apply --directory=dependencies/subversion ./dependencies/patches/subversion.diff

# Build node-apr-gen-test-char
cd dependencies/node-apr-gen-test-char
npm install
cd ../..

# Generate subversion project files
cd dependencies/subversion
python gen-make.py --with-zlib=../zlib --with-apr=../apr --with-apr-util=../apr --with-apr-iconv=../apr-iconv --with-sqlite=../sqlite-amalgamation --disable-shared --with-static-apr --vsnet-version=15 --disable-gmock
cd ../..

# Install Node dependencies
npm install

# Run configure script
npm run configure

# Build
npm run build

# Tests
npm run test
````
