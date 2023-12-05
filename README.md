YeOldeDinkEdit
==============

Or Yedit for short. A fork of Freedinkedit 109.x with a Dear ImGui interface. At the moment is very rough and effectively in a pre-release state, but fixes a whole ton of bugs from Freedinkedit including:

* Hard.dat is now read from and written to properly, rather than omitting tile screen 41
* Will now create MAP.DAT if it doesn't exist
* PNG tiles and sprites are loaded without filename hacks
* A few extra goodies such as the ability to hit backspace in tile mode to go back to your last tile screen

Building
========

Yedit uses the Meson build system. After installing it from your operating system's package manager, and then installing the relevant SDL2 dev packages including Mixer, TTF, GFX and Image, along with Gettext/Libintl, freetype2 and fontconfig, clone the repo and cd yedit then run:

```
meson setup builddir
meson compile -C builddir

# Run in windowed mode
builddir/yedit -w
```

Should build on macOS, GNU/Linux, and MSYS2 for Windows.

Credits
=======

A big thank you to Beuc for originally porting DinkEdit, and TealCool for suggesting features and testing.

Licensing
=========

     This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. 

    