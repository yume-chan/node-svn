#!/usr/bin/env bash

PREFIX=/var/tmp/build
BASE="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

mkdir -p ${PREFIX}

# libexpat
echo "start building libexpat..."
cd libexpat
mkdir -p make/macOS
cd libexpat/expat
./buildconf.sh
./configure --prefix=${PREFIX}
cd lib
make > ${BASE}/libexpat/make/macOS/make.txt
make install > ${BASE}/libexpat/make/macOS/make_install.txt

# apr
echo
echo "start building apr..."
cd ${BASE}/apr
mkdir -p make/macOS
cd apr
./buildconf
./configure --prefix=${PREFIX} --with-expat=${PREFIX}
make > ${BASE}/apr/make/macOS/make.txt
make install > ${BASE}/apr/make/macOS/make_install.txt

# openssl
echo
echo "start building openssl..."
cd ${BASE}/openssl/openssl
./Configure darwin64-x86_64-cc --prefix=${PREFIX}
make
make install

# serf
echo
echo "start building serf..."
cd ${BASE}/serf
mkdir -p make/macOS
cd serf
scons APR=${PREFIX} APU=${PREFIX} OPENSSL=${PREFIX} PREFIX=${PREFIX} > ${BASE}/serf/make/macOS/make.txt
scons install > ${BASE}/serf/make/macOS/make_install.txt

# subversion
echo
echo "start building subversion..."
cd ${BASE}/subversion
mkdir -p make/macOS
cd subversion
./autogen.sh
./configure --prefix=${PREFIX} --with-apr=${PREFIX} --with-apr-util=${PREFIX} --with-serf=${PREFIX} --disable-shared --with-lz4=internal --with-utf8proc=internal
make > ${BASE}/subversion/make/macOS/make.txt
make install > ${BASE}/subversion/make/macOS/make_install.txt
cp ${BASE}/subversion/subversion/subversion/libsvn_fs_fs/rep-cache-db.h ${BASE}/subversion/include/fs_fs/
cp ${BASE}/subversion/subversion/subversion/libsvn_fs_x/rep-cache-db.h ${BASE}/subversion/include/fs_x/
cp ${BASE}/subversion/subversion/subversion/libsvn_wc/wc-queries.h ${BASE}/subversion/include/wc/
