# SDL_Saturn
Simple Directmedia Layer, 1.2 branch for SEGA Saturn

This is an outdated port of SDL 1.2 by Hannu Viitala, Kimmo Kinnunen and
Markus Mertama.

Besides the use as module with the
[ngage-toolchain](https://github.com/ngagesdk/ngage-toolchain), this
repository also includes the demo application xflame by Carsten Haitzler
which was ported to SDL by Sam Lantinga.

Our SDL 2.0.x porting efforts can be found here:

https://github.com/ngagesdk/SDL

# DEPRECATED

The 1.2 branch of SDL is deprecated. While we occasionally collect fixes
in revision control, there has not been a formal release since 2012, and
we have no intention to do future releases, either.

Current development is happening in SDL 2.0.x, which gets regular
releases and can be found at:

https://github.com/libsdl-org/SDL

Thanks!

# How to Build

rm -rf * && cmake -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_TOOLCHAIN_FILE=$SATURN_CMAKE/sega_saturn.cmake \
      -DCMAKE_INSTALL_PREFIX=/saturn/SDL .. && make && make install

rm -rf * && cmake -DCMAKE_BUILD_TYPE=Debug \
      -DCMAKE_TOOLCHAIN_FILE=$SATURN_CMAKE/sega_saturn.cmake \
      -DCMAKE_INSTALL_PREFIX=/saturn/SDL .. && make && make install

# Simple DirectMedia Layer (SDL) Version 1.2

https://www.libsdl.org/

This is the Simple DirectMedia Layer, a general API that provides low
level access to audio, keyboard, mouse, joystick, 3D hardware via OpenGL,
and 2D framebuffer across multiple platforms.

The current version supports Linux, Windows CE/95/98/ME/XP/Vista, BeOS,
MacOS Classic, Mac OS X, FreeBSD, NetBSD, OpenBSD, BSD/OS, Solaris, IRIX,
and QNX.  The code contains support for Dreamcast, Atari, AIX, OSF/Tru64,
RISC OS, SymbianOS, Nintendo DS, and OS/2, but these are not officially
supported.

SDL is written in C, but works with C++ natively, and has bindings to
several other languages, including Ada, C#, Eiffel, Erlang, Euphoria,
Guile, Haskell, Java, Lisp, Lua, ML, Objective C, Pascal, Perl, PHP,
Pike, Pliant, Python, Ruby, and Smalltalk.

This library is distributed under GNU LGPL version 2, which can be
found in the file  "COPYING".  This license allows you to use SDL
freely in commercial programs as long as you link with the dynamic
library.

The best way to learn how to use SDL is to check out the header files in
the "include" subdirectory and the programs in the "test" subdirectory.
The header files and test programs are well commented and always up to date.
More documentation is available in HTML format in "docs/index.html".

The test programs in the "test" subdirectory are in the public domain.

Enjoy!

Sam Lantinga (slouken@libsdl.org)
