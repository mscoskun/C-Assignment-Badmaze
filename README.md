# C-Assignment-Badmaze

## About the Project
Badmaze is an interactive, console-based maze generation and pathfinding application developed as a first-year university assignment. The project specifications and target functionality were provided via a demonstration video.

While I developed the core game loop, user interface, and interactive grid system, I heavily utilized AI assistance to implement the advanced computer science algorithms—specifically the Depth-First Search (DFS) for maze generation and Breadth-First Search (BFS) for visual pathfinding. My primary focus was successfully integrating these complex structures into a seamless, animated console experience.

## Features
* **Interactive Builder:** Users can navigate the grid, manually draw walls, or erase blocks to create custom mazes.
* **Random Maze Generation:** Implements a randomized DFS algorithm to instantly generate complex, solvable mazes.
* **Visual Pathfinding:** Uses the BFS algorithm to calculate and visually animate the shortest path from the entrance to the exit.
* **File System:** Custom mazes can be saved to and loaded from local `.txt` files.
* **Dynamic Sizing:** Supports custom grid dimensions adjusted by the user.

## Controls
* `F1` - Draw Mode
* `F2` - Erase Mode
* `F3` - Navigation Mode
* `F4` - Fill Grid
* `F5 / F6` - Save / Load Maze
* `F7` - Auto-Solve (BFS Visualizer)
* `F8` - Clear Grid
* `F9` - Generate Random Maze (DFS)
* `F10` - Resize Grid
* `Space` - Pause Solver / `Esc` - Exit

## Built With
* **Language:** C
* **IDE:** Dev-C++

## Acknowledgments & The Closing Quote
A special thanks to **Prof. Dr. Alper Baştürk** for providing this challenging and highly educational assignment. 

The philosophical quote displayed when exiting the program was specifically added at the professor's request. Inspired by a similar feature in his own software, he encouraged us to leave a meaningful message for the user, adding a wonderful personal touch to this academic project.
