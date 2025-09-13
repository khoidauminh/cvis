## This is a C port of Coffeevis.

See the original project [here](https://github.com/khoidauminh/coffeevis_rs).

## Building

**NOTE: Cvis 0.3.0 has removed the terminal renderer**
**in order to support more advanced visualizers**

In the root directory (containing `CMakeLists.txt`):

```
cmake -B build
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
| \--size | 80 80 | sets resolution in window mode |
| \--fps | 60 | sets refresh rate (this disables vsync) |
| \--vis | spectrum | launches cvis with the specified visualizer |

## Keyboard Shortcuts

|  Key | Description |
| ------ | ------ |
| <kbd>Space</kbd> | iterates through visualizers (wraps around) |
| <kbd>q</kbd> | exits |
