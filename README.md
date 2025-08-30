## This is a C port of Coffeevis.

See the original project [here](https://github.com/khoidauminh/coffeevis_rs).

## Building

In the root directory (containing `meson.build`):

```
meson setup build
meson compile -C build
```

Cvis has support for Raylib, to use it:

```
meson setup -Draylib=true build
meson compile -C build
```

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
