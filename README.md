## This is a C port of Coffeevis.

See the original project [here](https://github.com/khoidauminh/coffeevis_rs).

## Building

**NOTE: CVIS 0.2.0 AND LATER USES CMAKE**

In the root directory (containing `CMakeLists.txt`):

```
cmake -B build
cmake --build build
```

Cvis has support for Raylib, to use it:

```
cmake -B build -Draylib=on
cmake --build build
```

Cvis can then be called in the `build` directory:

```
./build/cvis
```

## Archlinux PKGBUILD

Cvis also provides a PKGBUILD in `arch-build` that builds from the local repository.

## Commandline Arguments

| Option | Value (example) | Description |
| ------ | ------ | ------ |
| \--terminal | | runs in the terminal using ncurses |
| \--size | 80 80 | sets resolution in window mode |
| \--fps | 60 | sets refresh rate (this disables vsync) |
| \--vis | spectrum | launches cvis with the specified visualizer |

## Keyboard Shortcuts

|  Key | Description |
| ------ | ------ |
| <kbd>Space</kbd> | iterates through visualizers (wraps around) |
| <kbd>q</kbd> | exits |
