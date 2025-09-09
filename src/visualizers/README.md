## Coding Convention for Visualizers

- Each source file should only hold one visualizer. 

- Each visualizer must contain 1 function with the prefix `visualizer_` in name.

- Helper functions should be declared static.

- Static variables should be declared thread_local.