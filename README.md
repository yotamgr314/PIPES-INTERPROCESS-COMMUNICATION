
## Overview

- `loader.c` starts two child processes and connects them using two pipes.
- `child1` reads from `fileA.txt` and sends data to `child2` via a pipe.
- `child2` writes the received content to `fileB.txt` and replies back to `child1`.
- Once `child1` finishes, it sends `SIGUSR1` to the loader and exits.
- The loader listens for termination signals (`SIGCHLD`) and revives any child that exits.
- This revival can occur up to 5 times for `child1`, simulating a **self-healing system**.

## 🔧 Compilation

You need to compile the child programs and the loader:

```bash
gcc child1.c -o compiledFilesToLoad/A
gcc child2.c -o compiledFilesToLoad/B
gcc loader.c -o loader
