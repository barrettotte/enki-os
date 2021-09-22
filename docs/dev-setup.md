# dev-setup

## Cross Compiler

This could probably be a script...but its done once so I didn't bother.
Also, its just the steps on https://wiki.osdev.org/GCC_Cross-Compiler

### binutils

- `sudo apt install build-essential bison flex libgmp3-dev libmpc-dev libmpfr-dev texinfo`
- `export PREFIX="$HOME/opt/cross"`
- `export TARGET=i686-elf`
- `export PATH="$PREFIX/bin:$PATH"`
- `mkdir /tmp/src && cd /tmp/src`
- `curl -O http://ftp.gnu.org/gnu/binutils/binutils-2.36.1.tar.gz`
- `tar xf binutils-2.36.1.tar.gz`
- `mkdir binutils-build && cd binutils-build`
- `../binutils-2.36.1/configure --target=$TARGET --prefix=$PREFIX --with-sysroot --disable-nls --disable-werror 2>&1 | tee configure.log`
- `sudo make all install 2>&1 | tee make.log`

### gcc

- `cd /tmp/src`
- `curl -O https://ftp.gnu.org/gnu/gcc/gcc-11.1.0/gcc-11.1.0.tar.gz`
- `tar xf gcc-11.1.0.tar.gz`
- `mkdir gcc-build && cd gcc-build`
- `../gcc-11.1.0/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers`
- `make all-gcc`
- `make all-target-libgcc`
- `sudo make install-gcc`
- `sudo make install-target-libgcc`

### Verify Install and Add to Path

- Verify install - `$HOME/opt/cross/bin/$TARGET-gcc --version`
- permanent - `sudo nano ~/.bashrc`
  - add line at bottom - `export PATH="$HOME/opt/cross/bin:$PATH"`
- Reboot
