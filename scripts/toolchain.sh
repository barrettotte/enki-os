#!/bin/bash
# setup cross compiler for Enki OS
#
# Invoke with: make toolchain

# env
ARCH=i686
export PREFIX="$HOME/opt/cross"
export TARGET=$ARCH-elf
export PATH="$PREFIX:$PATH"

BINUTILS_V=2.38
GCC_V=11.2.0

# init directories
sudo mkdir -p $PREFIX
rm -rf /tmp/src/
mkdir -p /tmp/src

# install dependencies
sudo apt install -y build-essential bison flex libgmp3-dev libmpc-dev \
	libmpfr-dev texinfo libisl-dev

# fetch/init binutils
wget https://ftp.gnu.org/gnu/binutils/binutils-$BINUTILS_V.tar.xz -P /tmp/src
tar xvf /tmp/src/binutils-$BINUTILS_V.tar.xz -C /tmp/src
mkdir -p /tmp/src/build-binutils
cd /tmp/src/build-binutils
../binutils-$BINUTILS_V/configure --target=$TARGET --prefix="$PREFIX" \
	--with-sysroot --disable-nls --disable-werror

# build binutils
sudo make all install

# verify binutils installed
which -- $TARGET-as || exit

# fetch/init gcc
wget https://ftp.gnu.org/gnu/gcc/gcc-$GCC_V/gcc-$GCC_V.tar.xz -P /tmp/src
tar xvf /tmp/src/gcc-$GCC_V.tar.xz -C /tmp/src
mkdir -p /tmp/src/build-gcc
cd /tmp/src/build-gcc
../gcc-$GCC_V/configure --target=$TARGET --prefix="$PREFIX" \
	--disable-nls --enable-languages=c,c++ --without-headers

# build gcc
make all-gcc
make all-target-libgcc
sudo make install-gcc
sudo make install-target-libgcc

# verify gcc installed
$PREFIX/bin/$TARGET-gcc --version || exit

echo "Toolchain installed"
