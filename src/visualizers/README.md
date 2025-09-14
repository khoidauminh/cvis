## Coding Convention for Visualizers

- Each source file should only hold one visualizer. The main visualizer function should be named as follows:

```
[parent directory]_[visualizer name]
```

For example:

```
visualizer_spectrum
visualizer_scope
game_snake
```

- Helper functions should be declared static.

- Static variables should be declared       with `thread_local`.

- Visualizers should only use the available header files in the `include/` directory and NOT in the `internal/` subdirectory, as the latter are used only in the backend of cvis.
