YeOldeDinkEdit
==============

Or Yedit for short. A fork of Freedinkedit 109.x with a Dear ImGui interface. At the moment is very rough and effectively in a pre-release state, but fixes a whole ton of bugs from Freedinkedit including:

* Hard.dat is now read from and written to properly, rather than omitting tile screen 41
* Will now create MAP.DAT if it doesn't exist
* PNG tiles and sprites are loaded without filename hacks
* A few extra goodies such as the ability to hit backspace in tile mode to go back to your last tile screen

Building
========

Yedit uses the Meson build system. After installing it from your distro's package manager, and installing the relevant SDL2 dev packages including Mixer, TTF, GFX and Image, along with Gettext/Libintl, freetype2 and fontconfig, clone the repo and cd yedit then run:

```
meson setup builddir
meson compile -C builddir

# Run in windowed mode
builddir/yedit -w
```